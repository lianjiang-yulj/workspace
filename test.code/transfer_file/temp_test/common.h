#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
#include <fcntl.h>
#include <getopt.h>
#include <netinet/in.h>
#include <ev.h>
#include <stdio.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <arpa/inet.h> 
#include <string>
#include <sys/epoll.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstddef>
#include <unistd.h>

#include "Thread.h"
#include "ThreadPool.h"
#include "MMapMempool64.h"

int32_t PORT=-1;
int32_t BUFFER_SIZE=256;
int32_t IO_THREAD_CNT=-1;
std::string FILE_NAME;
util::file_desc fh;

util::thread::ThreadPool<util::thread::Thread>* thread_pool;
util::MMapMempool64 mmap_pool;

namespace common {

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

static void thread_read_cb(void* param);

static void
sigint_cb (struct ev_loop *loop, ev_signal *w, int revents) {
  printf("%d\n", revents);
  ev_break (loop, EVBREAK_ALL);
}


static int InitThreadPool() {
  thread_pool = new(std::nothrow) util::thread::ThreadPool<util::thread::Thread>(16, IO_THREAD_CNT, 1024 * 1024); 
}

static int DestroyThreadPool() {
  delete thread_pool;
}

static int LoadMapFile(std::string& file) {
  if(mmap_pool.Init(file.c_str()) != 0) return 1;

  util::thread::Thread* thread = thread_pool->getThread();
  fh.base = mmap_pool.get_base();
  fh.file_size = mmap_pool.get_file_size();
  thread->activate(util::MMapMempool64::PreLoad, &fh);
}


struct read_param {
  struct ev_loop *loop;
  struct ev_io *watcher;
  int revents;
};

static void print_usage(char *prog_name) {
  fprintf(stderr, "%s -p port -t io_thread_cnt -f file_path\n"
          "    -p, --port              server port\n"
          "    -t, --io_thread_cnt     thread count for listen, default: 1\n"
          "    -f, --file              input file path\n"
          "    -h, --help              display this help and exit\n"
          "    -V, --version           version and build time\n\n"
          "eg: %s -p 9999 -t 48 -f /home/lianjiang.yulj/in.txt\n\n", prog_name, prog_name);
}

static int parse_cmd_line(int argc, char *const argv[]) {
  int                     opt;
  const char              *opt_string = "hvp:t:f:";
  struct option           long_opts[] = {
    {"port", 1, NULL, 'p'},
    {"io_thread_cnt", 1, NULL, 't'},
    {"help", 0, NULL, 'h'},
    {"version", 0, NULL, 'V'},
    {0, 0, 0, 0}
  };

  opterr = 0;

  while ((opt = getopt_long(argc, argv, opt_string, long_opts, NULL)) != -1) {
    switch (opt) {
      case 'p':
        PORT = atoi(optarg);
        break;

      case 't':
        IO_THREAD_CNT = atoi(optarg);
        break;

      case 'f':
        FILE_NAME = optarg;
        break;

      case 'V':
        fprintf(stderr, "BUILD_TIME: %s %s\n", __DATE__, __TIME__);
        return opterr;

      case 'h':
        print_usage(argv[0]);
        return opterr;

      default:
        break;
    }
  }

  return opterr != 0;
}


static void setnonblock(int fd){
  fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) | O_NONBLOCK);
}
static void setreuseaddr(int fd){
  int ok=1;
  setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&ok,sizeof(ok));
}

static void setaddress(const char* ip,int port,struct sockaddr_in* addr){
  bzero(addr,sizeof(*addr));
  inet_pton(AF_INET,ip,&(addr->sin_addr));
  addr->sin_port=htons(port);
  addr->sin_family=AF_INET;
}

static std::string address_to_string(struct sockaddr_in* addr){
  char ip[128];
  inet_ntop(AF_INET,&(addr->sin_addr),ip,sizeof(ip));
  char port[32];
  snprintf(port,sizeof(port),"%d",ntohs(addr->sin_port));
  std::string r;
  r=r+"("+ip+":"+port+")";
  return r;
}

static int new_tcp_server(int port){
  int fd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  setnonblock(fd);
  setreuseaddr(fd);
  sockaddr_in addr;
  setaddress("0.0.0.0",port,&addr);
  if (bind(fd,(struct sockaddr*)&addr,sizeof(addr)) != 0) return -1;
  if (listen(fd,64) < 0) return -1; // backlog = 64
  return fd;
}

static int new_tcp_client(const char* ip,int port){
  int fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  setnonblock(fd);
  sockaddr_in addr;
  setaddress(ip,port,&addr);
  connect(fd,(struct sockaddr*)(&addr),sizeof(addr));
  return fd;
}

static int run_server(int sd) {
  struct ev_io socket_accept;

  struct ev_loop *epoller = ev_loop_new (EVBACKEND_EPOLL | EVFLAG_NOENV);
  ev_io_init(&socket_accept, accept_cb, sd, EV_READ);
  ev_io_start(epoller, &socket_accept);
  ev_signal signal_watcher;
  ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start (epoller, &signal_watcher);
  ev_run(epoller, 0);

  ev_loop_destroy(epoller);

  close(sd);
}

//accept事件 的回调块
void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sd;

  //分派客户端的ev io结构
  struct ev_io *w_client = (struct ev_io*) malloc (sizeof(struct ev_io));

  //libev的错误处理
  if(EV_ERROR & revents)
  {
    printf("error event in accept");
    return;
  }

  //accept,普通写法
  client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_sd < 0)
  {
    printf("accept error");
    return;
  }

  printf("someone connected: %s.\n", address_to_string(&client_addr).c_str());

  //开始监听读事件了,有客户端信息就会被监听到
  ev_io_init(w_client, read_cb, client_sd, EV_READ);
  ev_io_start(loop, w_client);
}

static void thread_read_cb(void* param) {
  read_param* rp = (read_param*) param;
  struct ev_loop *loop = rp->loop;
  struct ev_io *watcher = rp->watcher;
  int revents = rp->revents;

  char buffer[BUFFER_SIZE];
  ssize_t read;

  if(EV_ERROR & revents)
  {
    printf("error event in read");
    return;
  }

  //recv普通socket写法
  read = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

  if(read < 0)
  {
    printf("read error");
    return;
  }

  //断开链接的处理,停掉evnet就可以,同时记得释放客户端的结构体!
  if(read == 0)
  {
    printf("someone disconnected.\n");
    ev_io_stop(loop,watcher);
    free(watcher);
    return;
  }
  else
  {
    printf("get the message:%s\n",buffer);
  }
/*
  DataPos dp;
  bool is_no_data = false;
  while(mmap_pool.data_queue_.dequeue(dp) == 2) {
    if (mmap_pool.data_queue_.is_over_) {
      //server 没有数据了
      is_no_data = true;
      break;
    }
    usleep(1);
  }

  if (is_no_data) {
    //server没有消息了
    *(int32_t*) buffer = 0;
    buffer[5] = '\0';
    printf("send:#%s#\n", buffer);
    send(watcher->fd, buffer, 4, 0);
  } else {
    //TODO handle it
    dp.pos_[dp.len_] = '\0';
    printf("send:#%s#\n", dp.pos_);
    send(watcher->fd, dp.pos_, dp.len_, 0);
  }
  */
  printf("send:#%s#\n", buffer);
  send(watcher->fd, buffer, read, 0);

  delete rp;
  bzero(buffer, read);
}

//read 数据事件的回调快
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents){
  util::thread::Thread* thread;
  while (NULL == (thread = thread_pool->getThread())) {
    printf("thread pool is full");
    usleep(1);
  }

  read_param* rp = new(std::nothrow) read_param;
  rp->loop = loop;
  rp->watcher = watcher;
  rp->revents = revents;
  thread->activate(thread_read_cb, rp);
}

}; // namespace common

#endif // _COMMON_H_
