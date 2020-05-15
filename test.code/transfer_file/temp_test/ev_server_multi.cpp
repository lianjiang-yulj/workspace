#include <stdio.h>
#include <netinet/in.h>
#include <ev.h>
#include <sys/types.h>   
#include <sys/socket.h>   
#include <string.h>
#include <sys/epoll.h>
#include <malloc.h>
#include <iostream>
#include <fcntl.h>
#include <getopt.h>
#include <ev.h>
#include <string>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstddef>
#include <unistd.h>

#include "Thread.h"
#include "ThreadPool.h"
#include "MMapMempool64.h"

int64_t file_offset = 0;
int64_t data_serial = -1; //TODO atomic
int32_t PORT = 9999;
int32_t BUFFER_SIZE = 1024;
int32_t IO_THREAD_CNT=-1;
std::string FILE_NAME;
util::file_desc fh;

util::thread::ThreadPool<util::thread::Thread>* thread_pool;
util::MMapMempool64 mmap_pool;


int32_t deal_data(unsigned char* buffer, DataPos& dp) {
  char* src = dp.pos_;
  int32_t src_len = dp.len_;
  int32_t buff_num = 0;
  int32_t begin = src_len / 3;
  int32_t mid = src_len / 3 + src_len / 3;
  uint32_t rowno = (uint32_t)dp.rowno_; //TODO int serial if int64 serial ?
  int32_t head_len = 4;//TODO int is 4, int64 is 8 ?
  int32_t tail_len = 8;

  /*
  for (int j = 0; j < head_len; j++) {
    buffer[buff_num++] = (unsigned char)(rowno&0xff);
    rowno >>= 8;
  }
  */
  *(uint32_t*)buffer = rowno;
  buff_num += head_len;
  buff_num += tail_len;

  for (int32_t i = src_len - 1; i >= 0; i--) {
    if (i >= mid || i < begin) {
      buffer[buff_num++] = src[i];
    }
  }
  //TODO add tail offset of file
  //while (dp.rowno_ != data_serial + 1) {
   // usleep(1);
  //}
 
  int64_t temp_file_offset = file_offset;
  /*
  int32_t tail_len = 8;
  for (int j = 0; j < tail_len; j++) {
    buffer[buff_num++] = (unsigned char)(temp_file_offset&0xff);
    temp_file_offset >>= 8;
  }
  */
  *(uint64_t*)(buffer + head_len) = temp_file_offset;
  buffer[buff_num++] = '\r';
  buffer[buff_num++] = '\n';

  int32_t serial_len = 0;
  rowno = (int32_t)dp.rowno_;
  do {
    serial_len++;
    rowno /= 10;

  }while(rowno > 0);

  file_offset += serial_len; // TOOD cal file_offset
  file_offset += (buff_num - head_len - tail_len);

  data_serial ++; //TODO atomic
  buffer[buff_num] = '\0';
  return buff_num;
}


void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

void read_cb_async(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb_sync(struct ev_loop *loop, struct ev_io *watcher, int revents);

void thread_read_cb(void* param);
void thread_read_cb_async(void* param);

void sigint_cb (struct ev_loop *loop, ev_signal *w, int revents) {
  //printf("%d\n", revents);
  ev_break (loop, EVBREAK_ALL);
}

int InitThreadPool() {
  thread_pool = new(std::nothrow) util::thread::ThreadPool<util::thread::Thread>(IO_THREAD_CNT, IO_THREAD_CNT, 1024 * 1024); 
}

int DestroyThreadPool() {
  delete thread_pool;
}

int LoadMapFile(std::string& file) {
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
  int fd;
};

void print_usage(char *prog_name) {
  fprintf(stderr, "%s -p port -t io_thread_cnt -f file_path\n"
          "    -p, --port              server port\n"
          "    -t, --io_thread_cnt     thread count for listen, default: 1\n"
          "    -f, --file              input file path\n"
          "    -h, --help              display this help and exit\n"
          "    -V, --version           version and build time\n\n"
          "eg: %s -p 9999 -t 48 -f /home/lianjiang.yulj/in.txt\n\n", prog_name, prog_name);
}

int parse_cmd_line(int argc, char *const argv[]) {
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




void setnonblock(int fd){
  fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) | O_NONBLOCK);
}

void setreuseaddr(int fd){
  int ok=1;
  setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&ok,sizeof(ok));
}

void setaddress(const char* ip,int port,struct sockaddr_in* addr){
  bzero(addr,sizeof(*addr));
  addr->sin_family=AF_INET;
  inet_pton(AF_INET,ip,&(addr->sin_addr));
  addr->sin_port=htons(port);
}

std::string address_to_string(struct sockaddr_in* addr){
  char ip[128];
  inet_ntop(AF_INET,&(addr->sin_addr),ip,sizeof(ip));
  char port[32];
  snprintf(port,sizeof(port),"%d",ntohs(addr->sin_port));
  std::string r;
  r=r+"("+ip+":"+port+")";
  return r;
}

int new_tcp_server(int port){
  int fd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  setnonblock(fd);
  setreuseaddr(fd);
  sockaddr_in addr;
  setaddress("0.0.0.0",port,&addr);
  if (bind(fd,(struct sockaddr*)&addr,sizeof(addr)) != 0) return -1;
  if (listen(fd,64) < 0) return -1; // backlog = 64
  return fd;
}

int run_server(int sd) {

  struct ev_io socket_accept;
  struct ev_loop *loop = ev_default_loop(0);
  // 初始化,这里监听了io事件,写法参考官方文档的
  ev_io_init(&socket_accept, accept_cb, sd, EV_READ);
  ev_io_start(loop, &socket_accept);

  ev_run(loop, 0);
  close(sd);
}

int run_server2(int sd) {
  struct ev_io socket_accept;
  struct ev_loop *epoller = ev_loop_new (EVBACKEND_EPOLL | EVFLAG_NOENV);
  ev_io_init(&socket_accept, accept_cb, sd, EV_READ);
  ev_io_start(epoller, &socket_accept);
  /*
  ev_signal signal_watcher;
  ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start (epoller, &signal_watcher);
  */
  ev_loop(epoller, 0);

  ev_loop_destroy(epoller);

  close(sd);
}

void thread_run_server(void* sd) {
  struct ev_io socket_accept;
  struct ev_loop *epoller = ev_loop_new (EVBACKEND_EPOLL | EVFLAG_NOENV);
  ev_io_init(&socket_accept, accept_cb, *(int*)sd, EV_READ);
  ev_io_start(epoller, &socket_accept);
  /*
  ev_signal signal_watcher;
  ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start (epoller, &signal_watcher);
  */
  ev_loop(epoller, 0);

  ev_loop_destroy(epoller);

  close(*(int*)sd);
}

int main(int argc, char* argv[])
{

  parse_cmd_line(argc, argv);
  if (PORT ==-1 || IO_THREAD_CNT == -1 || FILE_NAME.empty()) {
    print_usage(argv[0]);
    return 1;
  }

  InitThreadPool();

  if(LoadMapFile(FILE_NAME) != 0)
  {
    //printf("load file %s error.", FILE_NAME.c_str());
    return 1;
  }

  int sd = new_tcp_server(PORT);

  //run_server(sd);
  for (int i = 0; i < IO_THREAD_CNT-1; i++) {
    util::thread::Thread* thread;
    while (NULL == (thread = thread_pool->getThread())) {
      //   //printf("thread pool is full");
      usleep(1);
    }
    thread->activate(thread_run_server, &sd);
  }

  while(1) {
    usleep(1000000);
  }
  DestroyThreadPool();

  return 0;
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
    //printf("error event in accept");
    return;
  }

  //accept,普通写法
  client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_sd < 0)
  {
    //printf("accept error");
    return;
  }

  //printf("someone connected: %s.\n", address_to_string(&client_addr).c_str());

  //开始监听读事件了,有客户端信息就会被监听到
  ev_io_init(w_client, read_cb_sync, client_sd, EV_READ);
  ev_io_start(loop, w_client);
}
void thread_read_cb_async(void* param) {
  read_param* rp = (read_param*) param;
  struct ev_loop *loop = rp->loop;
  struct ev_io *watcher = rp->watcher;
  int revents = rp->revents;
  int fd = rp->fd;

  unsigned char buffer[BUFFER_SIZE];
  ssize_t read;

  if(EV_ERROR & revents)
  {
    //printf("error event in read");
    return;
  }

  //recv普通socket写法
  while((read = recv(fd, buffer, BUFFER_SIZE, 0)) < 0)
  {
    //printf("read error");
    continue;
  }

  //断开链接的处理,停掉evnet就可以,同时记得释放客户端的结构体!
  if(read == 0)
  {
    //printf("someone disconnected.\n");
    ev_io_stop(loop,watcher);
    //free(watcher);
    return;
  }
  else
  {
    //printf("get the req:%s\n",buffer);
  }

  /*
  //printf("send the message:%s\n",buffer);
  //原信息返回,也可以自己写信息,都一样.最后记得置零
  send(watcher->fd, buffer, read, 0);
  */
  
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
    *(uint32_t*)buffer = -1;
    *(uint64_t*)(buffer+4) = -1;
    buffer[12] = '\r';
    buffer[13] = '\n';
    buffer[14] = '\0';
    while(send(fd, buffer, 14, 0) < 0) {
      continue;
    }
    //printf("send: #%d|%s|%ld#\n", *(int32_t*)buffer, buffer+12, *(int64_t*)(buffer+4));
  }
  else {
    //TODO handle it
    int32_t send_len = dp.len_;
    send_len = deal_data(buffer, dp);
    while(send(fd, buffer, send_len, 0) < 0) {
      continue;
    }
    //printf("send: #%d|%s|%ld#\n", *(int32_t*)buffer, buffer+12, *(int64_t*)(buffer+4));
  }
  
  //TODO how to unmap some useless memory
  //munmap(dp.pos_, dp.len_);

  delete rp;
}


void thread_read_cb(void* param) {
  read_param* rp = (read_param*) param;
  struct ev_loop *loop = rp->loop;
  struct ev_io *watcher = rp->watcher;
  int revents = rp->revents;

  unsigned char buffer[BUFFER_SIZE];
  ssize_t read;

  if(EV_ERROR & revents)
  {
    //printf("error event in read");
    return;
  }

  //recv普通socket写法
  while((read = recv(watcher->fd, buffer, BUFFER_SIZE, 0)) < 0)
  {
    //printf("read error");
    continue;
  }

  //断开链接的处理,停掉evnet就可以,同时记得释放客户端的结构体!
  if(read == 0)
  {
    //printf("someone disconnected.\n");
    ev_io_stop(loop,watcher);
    //free(watcher);
    return;
  }
  else
  {
    //printf("get the req:%s\n",buffer);
  }

  /*
  //printf("send the message:%s\n",buffer);
  //原信息返回,也可以自己写信息,都一样.最后记得置零
  send(watcher->fd, buffer, read, 0);
  */
  
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
    *(uint32_t*)buffer = -1;
    *(uint64_t*)(buffer+4) = -1;
    buffer[12] = '\r';
    buffer[13] = '\n';
    buffer[14] = '\0';
    while(send(watcher->fd, buffer, 14, 0) < 0) {
      continue;
    }
    //printf("send: #%d|%s|%ld#\n", *(int32_t*)buffer, buffer+12, *(int64_t*)(buffer+4));
  }
  else {
    //TODO handle it
    int32_t send_len = dp.len_;
    send_len = deal_data(buffer, dp);
    while(send(watcher->fd, buffer, send_len, 0) < 0) {
      continue;
    }
    //printf("send: #%d|%s|%ld#\n", *(int32_t*)buffer, buffer+12, *(int64_t*)(buffer+4));
  }
  
  //TODO how to unmap some useless memory
  //munmap(dp.pos_, dp.len_);

  delete rp;
}

//read 数据事件的回调快
void read_cb_async(struct ev_loop *loop, struct ev_io *watcher, int revents){
  util::thread::Thread* thread;
  while (NULL == (thread = thread_pool->getThread())) {
 //   //printf("thread pool is full");
    usleep(1);
  }

  read_param* rp = new(std::nothrow) read_param;
  rp->loop = loop;
  rp->watcher = watcher;
  rp->revents = revents;
  rp->fd = watcher->fd;
  thread->activate(thread_read_cb_async, rp);
}

void read_cb_sync(struct ev_loop *loop, struct ev_io *watcher, int revents){
  read_param* rp = new(std::nothrow) read_param;
  rp->loop = loop;
  rp->watcher = watcher;
  rp->revents = revents;
  thread_read_cb(rp);
}

