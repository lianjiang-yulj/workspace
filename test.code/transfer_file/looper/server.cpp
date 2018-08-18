#include <signal.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <fstream>

#include "Buffer.h"
#include "common.h"
#include "fileReader.h"
#include "looper.h"
#include "server_handler.h"
using namespace std;


string filename = "/data/data";


bool stop = false;
struct accepter_arg {
  int port;
  looper * l;
  int looper_num;
  fileReader *f;
};

void *accepter(void *arg){
  accepter_arg * item = (accepter_arg*)arg;
  looper * l = item->l;
  int looper_num = item->looper_num;
  fileReader *f = item->f;

  int sock = listenOrDie(item->port);
  setNonBlocking(sock);
  int fd = epoll_create(512);
  epoll_event ev;
  ev.data.fd = sock; 
  ev.events = EPOLLIN;
  int ret = epoll_ctl(fd,EPOLL_CTL_ADD, sock,&ev);
  if(ret==-1){
    cout<<"epoll ctl add error"<<endl;
  }

  epoll_event events[4];
  int count = 0;
  while(!stop){
    int num = epoll_wait(fd, events,4,100);
    for(int i=0;i<num;i++){
      if(events[i].events & EPOLLIN){
        int client = 0;
        sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        while( (client = accept(events[i].data.fd,(sockaddr*)&client_addr, &len)) != -1){
          int optval = 1;
          setsockopt(client, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
          setNonBlocking(client);
          Server * server = new Server();
          server->setFD(client);
          server->setReadBuf(new Buffer());
          server->setWriteBuf(new Buffer());
          server->setReader(f);
          l[count% looper_num].addRead(server);
          count++;

        }

      }
    }
  }
  return NULL;

}

void * runner(void *arg){
  looper * l= (looper *)arg;
  l->loop();
  return NULL;
}


void * prereader(void *arg){
  pthread_detach(pthread_self()); 
  fileReader *f = (fileReader*) arg;
  f->init(filename);
  return NULL;
}


int main(int argc, char ** argv){

  if(argc!=3){
    cerr<<"usage :filename port"<<endl; 
    exit(1);
  }
  int port = atoi(argv[2]);
  filename = argv[1];

  fileReader f;

  // 预读文件
  pthread_t pid_reader;
  pthread_create(&pid_reader,NULL, prereader,&f);

  signal(SIGPIPE, SIG_IGN);

  int thread_num = 12 ;
  looper l[thread_num];

  pthread_t accept;
  accepter_arg arg;
  arg.l= l;
  arg.looper_num = thread_num;
  arg.f = &f;
  arg.port = port;

  //启动监听端口
  pthread_create(&accept,NULL,accepter, &arg);

  //工作线程
  pthread_t pid[thread_num];
  for(int i=0;i<thread_num;i++){
    pthread_create(&pid[i],NULL,runner,&l[i]);
  }

  for(int i=0;i<thread_num;i++){
    pthread_join(pid[i],NULL);
  }
  stop = true;
  pthread_join(accept,NULL);

  return 0;

}
