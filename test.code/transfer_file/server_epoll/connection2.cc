#include "connection.h"
#include "handle.h"
#include <string>

Connection::Connection(int32_t client_sock) {
  socket_ = client_sock;
  r_socket_buf_ = new(std::nothrow) SocketBuff(client_sock, 4*1024, 5*1024*1024);
  w_socket_buf_ = new(std::nothrow) SocketBuff(client_sock, 4*1024, 5*1024*1024);
}

Connection::~Connection() {}

void Connection::read(EpollWorker* epoll_worker, epoll_event * ev){
  int32_t rc = r_socket_buf_->RecvSocket();
  if (rc == 0) {
    printf("client: %d close", socket_);
    //epoll_worker->DelReadEv(ev, socket_);
    close(socket_);
    return;
  } else if (rc < 0) {
 //   printf("client: %d recv -1\n", socket_);
    //TODO
    return;
  }


  //printf("pkg len = %d, send len = %d\n", size, rc);
  int32_t free_size = w_socket_buf_->GetFreeSize(); 
  int32_t size = handle_tcp_read(r_socket_buf_, w_socket_buf_->GetFreeData(), w_socket_buf_->GetFreeSize());
  assert(free_size >= size);
  rc = w_socket_buf_->WriteSocket(size);
  if (w_socket_buf_->GetLeftLen() == 0) {
    w_socket_buf_->Clear();
  } else if (w_socket_buf_->GetFreeSize() < 10240) {
    epoll_worker->AddWriteEv(ev, socket_);
    w_socket_buf_->ResetBuff();
  }

}


void Connection::write(EpollWorker* epoll_worker, epoll_event * ev) {

  if (w_socket_buf_->GetLeftLen() != 0) {
    int rc = w_socket_buf_->WriteSocket(0);
    //TODO if rc < 0 close()
  }
  if (w_socket_buf_->GetLeftLen() == 0) {
    w_socket_buf_->Clear();
    epoll_worker->DelWriteEv(ev, socket_);
  }
  return;
}
