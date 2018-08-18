#include <unistd.h>
#include "brick_queue.h"

BrickQueue::BrickQueue() {
  head_ = 0;
  tail_ = 0;
  brick_ring_size_ = kBrickRingMax - 1;
  brick_ring_ = new Brick[kBrickRingMax];
}

BrickQueue::~BrickQueue() {
}

int32_t BrickQueue::enqueue(const Brick& brick) {
  brick_ring_[head_] = brick;
  head_ = (head_ + 1)&(brick_ring_size_);
  con_id_ = brick.channel_id_;
  //printf("th %ld enqueue ok: line=%ld, head_ =%ld, tail_=%ld, con_id=%d \n", pthread_self(),line, head_, tail_, brick.channel_id_);
  return 0;
}

// brick_pkg_max_len 是建议的一次发送的大小
int32_t BrickQueue::dequeue(std::vector<Brick>& bricks,
                            int32_t brick_pkg_max_len,
                            int32_t& real_pkg_len) {

  int64_t cur_head = __sync_fetch_and_add(&head_, 0);
  while (tail_ == cur_head) {
	  usleep(1);
          //printf("th %ld dequeue fail, head=%ld, tail=%ld, con_id=%d\n",pthread_self(), cur_head, tail_, con_id_);
	  cur_head = __sync_fetch_and_add(&head_, 0);
  }
  while (cur_head != tail_) {
    //printf("th %ld dequeue ok, head=%ld, tail=%ld, con_id=%d\n",pthread_self(), cur_head, tail_, con_id_);
    real_pkg_len += brick_ring_[tail_].len_;
    bricks.push_back(brick_ring_[tail_]);
    tail_ = (tail_ + 1) & brick_ring_size_;
    if (real_pkg_len >= brick_pkg_max_len) break;
  }

  return 0;
}

