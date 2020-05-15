#ifndef SOCKET_BUFF_
#define SOCKET_BUFF_

#include <stdint.h>

class SocketBuff {
 public:
  SocketBuff(int32_t socket);
  virtual ~SocketBuff();

  bool NextPacket(char*& packet, int32_t& len);
 private:
  char* socket_buff_;
  char* data_;
  char* free_;
  int32_t data_len_;
  int32_t free_len_;
  int32_t socket_;

  int32_t read_size_;
  int32_t RecvSocket();
};

#endif  // SOCKET_BUFF_
