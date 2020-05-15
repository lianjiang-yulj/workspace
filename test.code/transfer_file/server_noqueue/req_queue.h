#ifndef REQQUEUE_H_
#define REQQUEUE_H_

#include <stdint.h>
#include "condition.h"

const int32_t kReqRingMax = 1UL << 1;

class Req {
 public:
  int32_t channel_id_;
  int32_t connection_id_;
};

class ReqQueue {
 public:
  ReqQueue();
  virtual ~ReqQueue();

  int32_t enqueue(const Req& req);
  int32_t dequeue(Req& req);

 private:
  Req* req_ring_;
  int32_t head_;
  int32_t tail_;
  util::thread::Condition cond_;
  int32_t req_ring_size_;
};

#endif  // REQQUEUE_H_

