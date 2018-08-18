#include "connection.h"
#include <string>

Connection::Connection(int32_t client_sock, CIOProcess* process) : process_(process) {
  socket_ = client_sock;
  r_socket_buf_ = new(std::nothrow) SocketBuff(client_sock, 8*1024, 5*1024*1024);
  w_socket_buf_ = new(std::nothrow) SocketBuff(client_sock, 4*1024, 5*1024*1024);
}

Connection::~Connection() {}

int Connection::read(EpollWorker* epoll_worker, epoll_event * ev){
  int32_t rc = r_socket_buf_->RecvSocket();
  if (rc == 0) {
    printf("client: %d close\n", socket_);
 //   epoll_worker->DelReadEv(ev, socket_);
    close(socket_);
    return -1;
  } else if (rc < 0) {
 //   printf("client: %d recv -1\n", socket_);
    //TODO
    return 0;
  }

 // epoll_worker->AddReadEv(ev, socket_);
  rc = process_->recv(r_socket_buf_);
  if (rc < 0) {
      close(socket_);
 //     fprintf(stderr, "client %d close\n", socket_);
      return rc;
  }
}


void Connection::write(EpollWorker* epoll_worker, epoll_event * ev) {

  while (w_socket_buf_->GetLeftLen() != 0) {
    int32_t rc = w_socket_buf_->WriteSocket(0);
    if (rc < 0) break;
  }
  w_socket_buf_->Clear();

  int size = process_->send(w_socket_buf_->GetFreeData(), w_socket_buf_->GetFreeSize());
  if (size > 0) {
      w_socket_buf_->WriteSocket(size);
      if (w_socket_buf_->GetLeftLen() != 0) {
 //         epoll_worker->AddWriteEv(ev, socket_);
          if (w_socket_buf_->GetFreeSize() < 2048) {
              w_socket_buf_->ResetBuff();
          }   
      } else {
          w_socket_buf_->Clear();
      }
  }


  return;
}
