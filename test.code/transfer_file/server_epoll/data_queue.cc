#include "data_queue.h"

//const int MAX_SIZE = 1UL<<28;
const int MAX_SIZE = 40000000;

DataQueue::DataQueue() {
  is_over_ = true;
  queue_num_ = 0;
  data_queue_list_ = new DataPos[MAX_SIZE];
  cur_line_ = 0;
}

DataQueue::~DataQueue() {
}

int32_t DataQueue::enqueue(DataPos& datap) {
  data_queue_list_[datap.rowno_] = datap;
  ++queue_num_;

  return 0;
}

int32_t DataQueue::dequeue(DataPos& datap, uint32_t& curline) {
  uint32_t line = __sync_fetch_and_add(&cur_line_, 1);
  curline = line;
  if (line >= queue_num_) return 2;
  datap = data_queue_list_[line];
  return 0;
}


int32_t DataQueue::dequeue(uint32_t line, DataPos& datap) {
  if (line >= queue_num_) return 2;
  datap = data_queue_list_[line];
  return 0;
}

void DataQueue::terminate()
{
  is_over_ = true;
}

void DataQueue::dump() {
/*
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
*/
}


