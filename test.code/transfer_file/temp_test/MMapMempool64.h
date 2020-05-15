#ifndef _NEWSN_UTIL_MMAPMEMPOOL64_H_
#define _NEWSN_UTIL_MMAPMEMPOOL64_H_

#include "stdint.h"
#include "data_queue.h"

namespace util {

struct file_desc {
  char* base;
  uint64_t file_size;
};

class MMapMempool64 {
 public:
  MMapMempool64();
  ~MMapMempool64();

  int32_t Init(const char* file_name);
  static void PreLoad(void* fh);

  inline char* get_base() {
    return base_;
  }
  inline uint64_t get_file_size() {
    return file_size_;
  }
  static DataQueue     data_queue_;

 private:
  int32_t        fd_; 
  char*          base_; 
  uint64_t      file_size_;


  static const uint64_t kMMapSize; 

  int32_t LoadFromFile(const char* file_name);
};



}

#endif
