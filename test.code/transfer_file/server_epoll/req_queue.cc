#include "req_queue.h"
#include <unistd.h>

ReqQueue::ReqQueue() {
  head_ = 0;
  tail_ = 0;
  req_ring_size_ = kReqRingMax - 1;
  req_ring_ = new Req[kReqRingMax];
}

ReqQueue::~ReqQueue() {
}

int32_t ReqQueue::enqueue(const Req& req) {
  int64_t line = __sync_fetch_and_add(&head_,1);
  if (line >= kReqRingMax) {
    return 0;
  }
  req_ring_[line] = req;

  return 0;
}

int32_t ReqQueue::dequeue(Req& req) {
  int64_t cur_head = __sync_fetch_and_add(&head_, 0);
  while (tail_ == cur_head) {
          usleep(1);
          //printf("th %ld dequeue fail, head=%ld, tail=%ld, con_id=%d\n",pthread_self(), cur_head, tail_, con_id_);
          cur_head = __sync_fetch_and_add(&head_, 0);
  }
  req = req_ring_[tail_];
  tail_++;

  return 0;
}

