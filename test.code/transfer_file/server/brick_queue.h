#ifndef BRICKQUEUE_H_
#define BRICKQUEUE_H_

#include <stdint.h>
#include <vector>

#include "condition.h"

const int32_t kBrickRingMax = 256;

class Brick {
 public:
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
  Brick brick_ring_[kBrickRingMax];
  int32_t head_;
  int32_t tail_;
  util::thread::Condition cond_;
  int32_t brick_ring_size_;
};

#endif  // BRICKQUEUE_H_

