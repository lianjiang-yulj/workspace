#include "data_queue.h"

DataQueue::DataQueue() {
  is_over_ = false;
  queue_num_ = 0;
}

DataQueue::~DataQueue() {
}

int32_t DataQueue::enqueue(DataPos& datap) {
  lock_.lock();

  data_queue_list_.push_back(datap);
  ++queue_num_;

  if (queue_num_ != 1) {
  }
  else {
    lock_.signal();
  }

  lock_.unlock();

  return 0;
}

int32_t DataQueue::dequeue(DataPos& datap) {
  lock_.lock();
  if (queue_num_ == 0) {
    if (is_over_) {
      lock_.unlock();
      return 2;
    }
    lock_.wait();
    if(queue_num_ == 0) {
      lock_.unlock();
      return 2;
    }
  }

  datap = data_queue_list_.front();
  data_queue_list_.pop_front();
  --queue_num_;
  lock_.unlock();

  return 0;
}

void DataQueue::terminate()
{
  lock_.lock();
  is_over_ = true;
  lock_.broadcast();
  lock_.unlock();
}

void DataQueue::dump() {
  printf("begin dump\n");
  char buf[1024];
  for (typeof(data_queue_list_.begin()) it = data_queue_list_.begin();
       it != data_queue_list_.end();
       ++it) {
    strncpy(buf,(*it).pos_, (*it).len_);
    buf[(*it).len_] = '\0';
    printf("%ld:%s\n",(*it).rowno_, buf);
  }
  printf("end dump\n");
}


