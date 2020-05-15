#include "brick_queue.h"

BrickQueue::BrickQueue() {
  head_ = 0;
  tail_ = 0;
  brick_ring_size_ = kBrickRingMax - 1;
}

BrickQueue::~BrickQueue() {
}

int32_t BrickQueue::enqueue(const Brick& brick) {
  cond_.lock();

  bool need_signal = (head_ == tail_);
  brick_ring_[tail_] = brick;
  tail_ = (tail_ + 1) % kBrickRingMax;
  //tail_ = (tail_ + 1) & brick_ring_size_;

  if (need_signal) {
    cond_.signal();
  }

  cond_.unlock();

  return 0;
}

// brick_pkg_max_len 是建议的一次发送的大小
int32_t BrickQueue::dequeue(std::vector<Brick>& bricks,
                            int32_t brick_pkg_max_len,
                            int32_t& real_pkg_len) {
  cond_.lock();

  while (head_ == tail_) {
    cond_.wait();
  }

  real_pkg_len = 0;
  while (head_ != tail_) {
    real_pkg_len += brick_ring_[head_].len_;
    bricks.push_back(brick_ring_[head_]);
    head_ = (head_ + 1) % kBrickRingMax;
    //head_ = (head_ + 1) & brick_ring_size_;
    if (real_pkg_len >= brick_pkg_max_len) break;
  }

  cond_.unlock();

  return 0;
}

