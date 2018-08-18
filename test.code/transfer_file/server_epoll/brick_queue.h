#ifndef BRICKQUEUE_H_
#define BRICKQUEUE_H_

#include <stdint.h>
#include <vector>

#include "condition.h"

const int32_t kBrickRingMax = 1UL << 10;

class Brick {
 public:
  Brick() {len_ = -1;}
  int32_t channel_id_;
  char* brick_;
  int32_t len_;
};

class BrickQueue {
 public:
  BrickQueue();
  virtual ~BrickQueue();

  int32_t enqueue(const Brick& brick);
  int32_t dequeue(std::vector<Brick>& bricks,
                  int32_t brick_pkg_max_len,
                  int32_t& real_pkg_len);

 private:
  Brick* brick_ring_;//[kBrickRingMax];
  int64_t head_;
  int64_t tail_;
  util::thread::Condition cond_;
  int64_t brick_ring_size_;
  int con_id_;
};

#endif  // BRICKQUEUE_H_

