#ifndef DATAQUEUE_H_
#define DATAQUEUE_H_

#include <list>
#include <stdint.h>

#include "condition.h"

class DataPos {
 public:
  char* pos_;
  uint8_t len_;
  uint32_t rowno_;
};

class DataQueue {
 public:
  DataQueue();
  virtual ~DataQueue();

  int32_t enqueue(DataPos& datap);
  int32_t dequeue(DataPos& datap, uint32_t& curline);
  int32_t dequeue(uint32_t line, DataPos& datap);

  void terminate();
  void dump();

  bool is_over_;
  DataPos* data_queue_list_;
  util::thread::Condition lock_;
  volatile uint32_t queue_num_;
  uint32_t cur_line_;
};

#endif  // DATAQUEUE_H_

