#ifndef DATAQUEUE_H_
#define DATAQUEUE_H_

#include <list>

#include "Condition.h"

class DataPos {
 public:
  char* pos_;
  int32_t len_;
  int64_t rowno_;
};

class DataQueue {
 public:
  DataQueue();
  virtual ~DataQueue();

  int32_t enqueue(DataPos& datap);
  int32_t dequeue(DataPos& datap);

  void terminate();
  void dump();

  bool is_over_;
  std::list<DataPos> data_queue_list_;
  util::thread::Condition lock_;
  int64_t queue_num_;
};

#endif /* DATAQUEUE_H_ */

