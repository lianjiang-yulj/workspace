#include "epoll_worker.h"
#include <string.h>

#include "connection.h"
#include "../server_epoll/common.h"

EpollWorker::EpollWorker() {
  fd_ = epoll_create(512);
}

EpollWorker::~EpollWorker() {
} 

void EpollWorker::AddClient(int32_t client_sock, CIOProcess* process) {
  int32_t optval = 1;
  setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval)));
  set_socket_max_buffer(client_sock);
  setnonblock(client_sock);
  Connection* con = new(std::nothrow) Connection(client_sock, process);
  
  epoll_event event;
  event.data.ptr = con;
  event.events = EPOLLIN | EPOLLOUT;

  int32_t ret = epoll_ctl(fd_, EPOLL_CTL_ADD, client_sock, &event);
  if(ret == -1){
    printf("add %s\n", strerror(errno));
  }
}

void EpollWorker::AddClient(int32_t client_sock) {
  int32_t optval = 1;
  setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval)));
  set_socket_max_buffer(client_sock);
  setnonblock(client_sock);
}


void EpollWorker::AddWriteEv(epoll_event * ev, int32_t fd){
  ev->events |= EPOLLOUT;  
  epoll_ctl(fd_, EPOLL_CTL_MOD, fd, ev);
  return;
}

void EpollWorker::DelWriteEv(epoll_event * ev, int32_t fd){
  ev->events &=  ~EPOLLOUT;  
  epoll_ctl(fd_, EPOLL_CTL_MOD, fd, ev);
  return;
}

void EpollWorker::DelReadEv(epoll_event * ev, int32_t fd){
  ev->events &=  ~EPOLLIN;  
  epoll_ctl(fd_, EPOLL_CTL_MOD, fd, ev);
  return;
}

void EpollWorker::AddReadEv(epoll_event * ev, int32_t fd){
  ev->events |=  EPOLLIN;  
  epoll_ctl(fd_, EPOLL_CTL_MOD, fd, ev);
  return;
}



void EpollWorker::run() {
  int32_t event_num = 512;
  epoll_event ev[event_num];
  while(true) {
    int32_t num = epoll_wait(fd_, ev, event_num, 100);
    for(int32_t i = 0; i < num; i++){
      Connection* con = (Connection*) ev[i].data.ptr;
      if(ev[i].events & EPOLLIN) {
        if (con->read(this, &ev[i]) < 0) {
 //           fprintf(stderr, "exit....\n");
            return;
        }
      }

      if(ev[i].events & EPOLLOUT) {
        con->write(this, &ev[i]); 
      }
    }
  }
  return;
}
