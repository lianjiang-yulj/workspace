#ifndef REQQUEUE_H_
#define REQQUEUE_H_

#include <stdint.h>
#include <vector>

#include "datatype.h"

const int32_t kReqRingMax = 1UL << 12;

class ReqQueue {
 public:
  ReqQueue();
  virtual ~ReqQueue();

  int32_t enqueue(const SRequestItem& req);
  int32_t dequeue(std::vector<SRequestItem>& reqVec);

 private:
  SRequestItem* req_ring_;
  bool* req_flag_;
  //bool* volatile req_flag_;
  int32_t head_;
  int32_t tail_;
  int32_t req_ring_size_;
};

#endif  // REQQUEUE_H_

