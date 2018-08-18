#include "socket_buff.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

const int32_t kSize = 8 * 1024;
const int32_t kFreeSize = 4 * 1024;
const int32_t kMaxSize = 128 * 1024;

SocketBuff::SocketBuff(int32_t socket)
  : socket_(socket){
    socket_buff_ = (char*) malloc(kMaxSize);
    data_ = socket_buff_;
    free_ = socket_buff_;
    data_len_ = 0;
    free_len_ = kMaxSize;
  }

SocketBuff::~SocketBuff() {
}

// 0: client close
// 1: recv success
// -1: recv fail
int32_t SocketBuff::RecvSocket() {
  int32_t recv_size = 0;
  do {
    recv_size = ::recv(socket_, free_, free_len_, 0);
    if (recv_size < 0) {
      continue;
    } else if (recv_size == 0) {
      return 0;
    } else {
      data_len_ += recv_size;
      free_len_ -= recv_size;
      free_ += recv_size;
      if (free_len_ < kFreeSize) {
        memmove(socket_buff_, data_, data_len_);
        data_ = socket_buff_;
        free_ = data_ + data_len_;
        free_len_ = kMaxSize - data_len_;
      }
      // 确保收到header
      if (data_len_ >= 2) {
        uint16_t packet_len = *(uint16_t*) data_;
        if (packet_len <= data_len_) {
          break;
        }
      }
    }
  } while(true);

  return 1;
}

bool SocketBuff::NextPacket(char*& packet, int32_t& len) {
  if (data_ == free_) {
    if (!RecvSocket()) {
      return false;
    }
  }

  if (data_len_ < 2 || (*(uint16_t*) data_) > data_len_) {
    if (!RecvSocket()) {
      return false;
    }
  }

  // 校验
  if (data_len_ < 2 || (*(uint16_t*) data_) > data_len_) {
    return false;
  }

  uint16_t packet_len = *(uint16_t*) data_;
  packet = data_;
  len = packet_len;
  data_len_ -= len;
  data_ += packet_len;

  return true;
}


