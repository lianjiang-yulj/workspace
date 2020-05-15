#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "epoll_worker.h"
#include "socket_buff.h"

class Connection  {
 public:
  Connection(int32_t client_sock);
  virtual ~Connection();


  void set_socket(int32_t socket) { socket_ = socket; }
  int32_t get_socket() { return socket_; }
  void read(EpollWorker* epoll_worker, epoll_event *ev); 
  void write(EpollWorker* epoll_worker, epoll_event *ev); 

 private:
  int32_t socket_;
  SocketBuff* r_socket_buf_;
  SocketBuff* w_socket_buf_;
};

#endif // CONNECTION_H_
