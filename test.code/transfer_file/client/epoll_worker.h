#ifndef EPOLL_WORKER_H_
#define EPOLL_WORKER_H_

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>

#include <sys/epoll.h>
#include <errno.h>
#include <queue>
#include "condition.h"

class Connection;

class EpollWorker {
 public:
  EpollWorker();
  virtual ~EpollWorker();

  void AddClient(int32_t client_sock);
  void run();
  void AddWriteEv(epoll_event * ev, int32_t fd);
  void DelWriteEv(epoll_event * ev, int32_t fd);
  void AddReadEv(epoll_event * ev, int32_t fd);
  void DelReadEv(epoll_event * ev, int32_t fd);
 private:
  int32_t fd_;
  std::queue<Connection* > con_client_;
  util::thread::Condition cond_;

};

#endif  // EPOLL_WORKER_H_
