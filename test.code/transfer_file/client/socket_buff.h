#ifndef SOCKET_BUFF_
#define SOCKET_BUFF_

#include <stdint.h>

class SocketBuff {
 public:
  SocketBuff(int32_t socket, int32_t read_size, int32_t buff_max_size);
  virtual ~SocketBuff();

  bool NextPacket(char*& packet, int32_t& len);
  int32_t RecvSocket();
  char* GetFreeData();
  int32_t GetFreeSize();
  int32_t WriteSocket(int32_t size);
  char* GetData();
  int32_t GetLeftLen();
  void Clear();
  void ResetBuff();
 private:
  char* socket_buff_;
  char* data_;
  char* free_;
  int32_t data_len_;
  int32_t free_len_;
  int32_t socket_;
  int32_t max_size_;
  int32_t read_size_;
  int32_t init_read_size_;

};

#endif  // SOCKET_BUFF_
