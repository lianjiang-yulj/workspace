#include "req_queue.h"

ReqQueue::ReqQueue() {
  head_ = 0;
  tail_ = 0;
  req_ring_size_ = kReqRingMax - 1;
}

ReqQueue::~ReqQueue() {
}

int32_t ReqQueue::enqueue(const Req& req) {
  cond_.lock();

  bool need_signal = (head_ == tail_);
  req_ring_[tail_] = req;
  tail_ = (tail_ + 1) % kReqRingMax;
  //tail_ = (tail_ + 1) & req_ring_size_;
  if (need_signal) {
    cond_.signal();
  }

  cond_.unlock();

  return 0;
}

int32_t ReqQueue::dequeue(Req& req) {
  cond_.lock();

  while (head_ == tail_) {
    cond_.wait();
  }
  req = req_ring_[head_];
  head_ = (head_ + 1) % kReqRingMax;
  //head_ = (head_ + 1) & req_ring_size_;

  cond_.unlock();

  return 0;
}

