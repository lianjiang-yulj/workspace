#include "socket_buff.h"

#include <malloc.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

SocketBuff::SocketBuff(int32_t socket, int32_t read_size, int32_t buff_max_size)
  : socket_(socket){
    socket_buff_ = (char*) malloc(buff_max_size);
    data_ = socket_buff_;
    free_ = socket_buff_;
    data_len_ = 0;
    free_len_ = buff_max_size;
    max_size_ = buff_max_size;
    read_size_ = read_size;
    init_read_size_ = read_size;
    if (read_size_ > buff_max_size) {
      read_size_ = buff_max_size;
      init_read_size_ = buff_max_size;
    }
  }

SocketBuff::~SocketBuff() {
}

// 0: client close
// 1: recv success
// -1: recv fail
int32_t SocketBuff::RecvSocket() {
  int32_t recv_size = 0;
  if (read_size_ > free_len_) read_size_ = free_len_;
  recv_size = ::read(socket_, free_, read_size_);
  if (recv_size < 0) {
    //printf("client: %d recv = %d, errno=%d, err=%s\n", socket_, recv_size, errno, strerror(errno));
    return -1;
  } else if (recv_size == 0) {
    //printf("client: %d recv = %d, errno=%d, err=%s\n", socket_, recv_size, errno, strerror(errno));
    return 0;
  } else {
    data_len_ += recv_size;
    free_len_ -= recv_size;
    free_ += recv_size;
    if (free_len_ < init_read_size_) {
      memmove(socket_buff_, data_, data_len_);
      data_ = socket_buff_;
      free_ = data_ + data_len_;
      free_len_ = max_size_ - data_len_;
      read_size_ = init_read_size_;
    }
  }

  return recv_size;
}

bool SocketBuff::NextPacket(char*& packet, int32_t& len) {
  if (data_ == free_) {
    return false;
  }

  if (data_len_ < 2 || (*(uint16_t*) data_) > data_len_) {
    return false;
  }

  uint16_t packet_len = *(uint16_t*) data_;
  packet = data_ + sizeof(uint16_t);
  len = packet_len - sizeof(uint16_t);
  data_len_ -= packet_len;
  data_ += packet_len;

  return true;
}

char* SocketBuff::GetFreeData() {
  return free_;
}

int32_t SocketBuff::GetFreeSize() {
  return free_len_;
}

int32_t SocketBuff::WriteSocket(int32_t size) {
  data_len_ += size;
  free_len_ -= size;
  int32_t rc = 0;
  if (data_len_ > 0) {
    rc = ::write(socket_, GetData(), GetLeftLen());
    if(rc > 0) {
      data_len_ -= rc;
      data_ += rc;
    }
    else if (rc < 0) {
      if (errno == ECONNRESET || errno == EPIPE) {
        close(socket_);
        printf("client reset socket\n");
      }
    }
  }
  return rc; 
}

char* SocketBuff::GetData() {
  return data_;
}

int32_t SocketBuff::GetLeftLen() {
  return data_len_;
}

void SocketBuff::Clear() {
  data_ = socket_buff_;
  free_ = socket_buff_;
  data_len_ = 0;
  free_len_ = max_size_;
}

void SocketBuff::ResetBuff() {
  memmove(socket_buff_, data_, data_len_);
  data_ = socket_buff_;
  free_ = data_ + data_len_;
  free_len_ = max_size_ - data_len_;
}
