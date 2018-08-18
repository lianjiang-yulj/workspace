#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "mmap_mempool64.h"


namespace util {

DataQueue     MMapMempool64::data_queue_;
const uint64_t MMapMempool64::kMMapSize           = 1UL<< 32;  // 16G

MMapMempool64::MMapMempool64()
    : fd_(-1),
    base_(NULL){
    }

MMapMempool64::~MMapMempool64() {
  if (base_ != NULL) {
    munmap(base_, kMMapSize);
    close(fd_);
  }
}

int32_t MMapMempool64::Init(const char* file_name) {
  if(file_name == NULL) {
    return -1;
  }

  int32_t ret = access(file_name, F_OK);
  if (ret != 0) return -1;
  if(LoadFromFile(file_name) < 0) {
    return -1;
  }

  return 0;
}


int32_t MMapMempool64::LoadFromFile(const char* file_name) {
  fd_ = open(file_name, O_RDONLY);
  if(fd_ < 0) {
    return -1;
  }

  base_ = (char*)mmap(NULL, kMMapSize, PROT_READ, /*MAP_POPULATE*/MAP_SHARED, fd_, 0);
  if(MAP_FAILED == base_) {
    return -1;
  }

  struct stat st;
  fstat(fd_, &st);
  file_size_ = st.st_size;

  posix_madvise(base_, file_size_, POSIX_MADV_SEQUENTIAL);

  return 0;
}

void MMapMempool64::PreLoad(void* fh) {
  char* base;
  uint64_t file_size;
  file_desc* fd = (file_desc*) fh;
  base = fd->base;
  file_size = fd->file_size;

  struct timeval tv_begin, tv_end;
  gettimeofday(&tv_begin, NULL);
  printf("load begin\n");

  /*
     if(file_size > 0) {
     int32_t buf_size = 4*1024;
     char* buf = new(std::nothrow) char[buf_size];
     uint64_t max_size = (file_size / buf_size) * buf_size;
     uint64_t size = 0;
     while(size < max_size) {
     memcpy(buf, base+ size, buf_size);
     size += buf_size;
     }
     if(file_size > max_size) {
     memcpy(buf, base+ max_size, file_size - max_size);
     }
     delete [] buf;
     }
     */
  if (file_size > 0) {
    int64_t begin = 0;
    int64_t rowno = 0;
    for (uint64_t i = begin; i < file_size;) {
      if (base[i] == '\n') {
        DataPos dp;
        dp.pos_ = base + begin;
        dp.len_ = i - begin;
        dp.rowno_ = (rowno++);
        i += 1;
        begin = i;
        data_queue_.enqueue(dp);

      } else if (base[i] == '\r' && base[i+1] == '\n') {
        DataPos dp;
        dp.pos_ = base + begin;
        dp.len_ = i - begin;
        dp.rowno_ = (rowno++);
        i += 2;
        begin = i;
        data_queue_.enqueue(dp);
      }
      else {
        ++i;
      }
    }
  }
  data_queue_.terminate();
  gettimeofday(&tv_end, NULL);
  printf("load end\n");
  int64_t tc = tv_end.tv_sec - tv_begin.tv_sec + (tv_end.tv_usec - tv_begin.tv_usec) / 1000000;
  printf("load cost: %ld\n", tc);
  // data_queue_.dump();
}

void* MMapMempool64::PreLoadFile(void* fh) {
  char* base;
  uint64_t file_size;
  file_desc* fd = (file_desc*) fh;
  base = fd->base;
  file_size = fd->file_size;

  struct timeval tv_begin, tv_end;
  gettimeofday(&tv_begin, NULL);
  printf("load begin\n");

  int64_t begin = 0;
  int64_t rowno = 0;
  if (file_size > 0) {
    for (uint64_t i = begin; i < file_size;) {
      if (base[i] == '\n') {
        DataPos dp;
        dp.pos_ = base + begin;
        dp.len_ = i - begin;
        dp.rowno_ = (rowno++);
        i += 1;
        begin = i;
        data_queue_.enqueue(dp);

      } else if (base[i] == '\r' && base[i+1] == '\n') {
        DataPos dp;
        dp.pos_ = base + begin;
        dp.len_ = i - begin;
        dp.rowno_ = (rowno++);
        i += 2;
        begin = i;
        data_queue_.enqueue(dp);
      } else if (i == file_size - 1) {
        DataPos dp;
        dp.pos_ = base + begin;
        dp.len_ = i - begin + 1;
        dp.rowno_ = (rowno++);
        i += 1;
        begin = i;
        data_queue_.enqueue(dp);
      }
      else {
        ++i;
      }
    }
  }
  data_queue_.terminate();
  gettimeofday(&tv_end, NULL);
  printf("load end\n");
  int64_t tc = tv_end.tv_sec - tv_begin.tv_sec + (tv_end.tv_usec - tv_begin.tv_usec) / 1000000;
  printf("load cost: %lds, rowcount: %ld\n", tc, rowno);
 // fflush(stdout);
  // data_queue_.dump();
  return NULL;
}

}

