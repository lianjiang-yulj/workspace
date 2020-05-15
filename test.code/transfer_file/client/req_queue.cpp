#include "req_queue.h"
#include <unistd.h>

using std::vector;

const int MAX_TRY = 100;

ReqQueue::ReqQueue() {
  head_ = 0;
  tail_ = 0;
  req_ring_ = new SRequestItem[kReqRingMax];
  req_flag_ = new bool[kReqRingMax];
  for (int i = 0; i < kReqRingMax; ++i) {
  	req_flag_[i] = false;
  }
  req_ring_size_ = kReqRingMax - 1;
}

ReqQueue::~ReqQueue() {
  delete[] req_ring_;
  delete[] req_flag_;
}

int32_t ReqQueue::enqueue(const SRequestItem& req) {
  int32_t line = __sync_fetch_and_add(&head_,1);
  line = line & req_ring_size_;
  req_ring_[line] = req;
  req_flag_[line] = true;

  return 0;
}

int32_t ReqQueue::dequeue(vector<SRequestItem>& reqVec) {
  int32_t cur_head = __sync_fetch_and_add(&head_, 0);
  int i = 0;
  while (tail_ == cur_head || !req_flag_[tail_]) {
          if (++i > MAX_TRY) break;
          usleep(1);
          //printf("th %ld dequeue fail, head=%ld, tail=%ld, con_id=%d\n",pthread_self(), cur_head, tail_, con_id_);
          cur_head = __sync_fetch_and_add(&head_, 0);
  }
  while (tail_ < cur_head && req_flag_[tail_]) {
	req_flag_[tail_] = false;
	reqVec.push_back(req_ring_[tail_]);
	tail_ = (tail_ + 1) & req_ring_size_;
  }

  return 0;
}

