//
//  多线程版Hello World服务
//
//#include "zhelpers.h"
#include <zmq.h>
#include <malloc.h>
#include <string>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>

#include <vector>
#include <list>
#include "Condition.h"
#include "../MMapMempool64.h"

int pkg_top_cnt = 0;

class MsgQ{
 public:
  MsgQ()
  : msg_num_(0), req_num_(0) {
  }

  virtual ~MsgQ() {}

  int32_t req_enqueue(int32_t req_num) {
    req_lock_.lock();

    if (req_num_ == 0) {
      req_num_ += req_num;
      req_lock_.signal();
    } else {
      req_num_ += req_num;
    }

    req_lock_.unlock();

    return req_num;
  }

  // dequeue 1 ~ max req and return it
  int32_t req_dequeue(int32_t max_req) {
    req_lock_.lock();

    while (req_num_ < 1) {
      req_lock_.wait();
    }

    int reqs = max_req;
    if (req_num_ >= max_req) {
      reqs = max_req;
    } else {
      reqs = req_num_;
    }

    req_num_ -= reqs;

    req_lock_.unlock();

    return reqs;
  }


  // enqueue one msg dealed
  int32_t msg_enqueue(DataPos& msg) {
    msg_lock_.lock();

    msg_queue_.push_back(msg);
    ++msg_num_;

    if (msg_num_ == 1) {
      msg_lock_.signal();
    }

    msg_lock_.unlock();

    return 1;
  }

  // enqueue some msgs dealed
  int32_t msg_enqueue(std::vector<DataPos>& msg) {
    msg_lock_.lock();

    for (int i = 0; i < msg.size(); i ++) {
      msg_queue_.push_back(msg[i]);
    }

    msg_num_ += msg.size();

    if (msg_num_ == msg.size()) {
      msg_lock_.signal();
    }

    msg_lock_.unlock();

    return msg.size();
  }


  int32_t msg_dequeue(std::vector<DataPos>& msg, int32_t msg_num) {
    msg_lock_.lock();

    while (msg_num_ < msg_num) {
      msg_lock_.wait();
    }
    for (int m = 0; m < msg_num; m++) {
      msg.push_back(msg_queue_.front());
      msg_queue_.pop_front();
    }
    msg_num_ -= msg_num;

    msg_lock_.unlock();

    return msg_num;
  }

  int32_t msg_dequeue(DataPos& msg) {
    msg_lock_.lock();

    while (msg_num_ < 1) {
      msg_lock_.wait();
    }
    msg = msg_queue_.front();
    msg_queue_.pop_front();
    msg_num_ -= 1;

    msg_lock_.unlock();

    return 1;
  }

 private:
  std::list<DataPos> msg_queue_;
  int64_t req_num_;
  util::thread::Condition msg_lock_;
  util::thread::Condition req_lock_;;
  int64_t msg_num_;
};

// 全局消息队列
MsgQ g_msgq;

// 从套接字后去ZMQ 多帧消息
static int
s_more_recv(void *socket, std::vector<DataPos>& msg) {
  zmq_msg_t message;

  do {
    zmq_msg_init (&message);
    zmq_recvmsg (socket, &message, 0);
    int size = zmq_msg_size (&message);
    DataPos dp;
    // TODO free it after client write it to file
    dp.pos_ = (char*) malloc (size + 1);
    memcpy (dp.pos_, zmq_msg_data (&message), size);
    dp.pos_[size] = '\0';

    msg.push_back(dp);
    /*
    int32_t more = 0;
    size_t more_size = sizeof (more);
    zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &more_size);
    if (!more)
      break; // 已到达最后一帧
    */
    bool bmore = zmq_msg_more(&message);
    if (!bmore) {
      breakk;
    }
  } while(1);

  zmq_msg_close (&message);

  return msg.size();
}
//  从套接字中获取ZMQ字符串，并转换为C语言字符串
static char *
s_recv (void *socket) {
  zmq_msg_t message;
  zmq_msg_init (&message);
  zmq_recvmsg (socket, &message, 0);
  int size = zmq_msg_size (&message);
  char *string = (char*) malloc (size + 1);
  memcpy (string, zmq_msg_data (&message), size);
  zmq_msg_close (&message);
  string [size] = 0;
  return (string);
}

//  将C语言字符串转换为ZMQ字符串，并发送给套接字
static int
s_send (void *socket, char *string, int size) {
  int rc;
  zmq_msg_t message;
  zmq_msg_init_size (&message, size);
  memcpy (zmq_msg_data (&message), string, size);
  rc = zmq_sendmsg (socket, &message, 0);
  zmq_msg_close (&message);
  return (rc);
}

static void *
worker_routine (void *context) {
  //  连接至代理的套接字
  void *receiver = zmq_socket (context, ZMQ_REQ);
  int rc = zmq_connect (receiver, "tcp://10.97.184.60:5555");
  assert(rc == 0);

  char buffer[256];
  memset(buffer, 'R', sizeof(buffer));

  std::vector<DataPos> msg;
  while (1) {
    // 发送请求
    // get some reqs
    int reqs = g_msgq.req_dequeue(pkg_top_cnt);
    rc = s_send (receiver, buffer, reqs);
    assert (rc == reqs);
    msg.clear();

    rc = s_more_recv (receiver, msg);
    assert(rc == reqs);

    g_msgq.msg_enqueue(msg);
    //  工作
    // TODO free msg after write to file
  }
  zmq_close (receiver);
  return NULL;
}

static void *
deal_routine (void *context) {
  while (true) {
    // 发送请求
    // send one req
    g_msgq.req_enqueue(1);
    DataPos dp;
    g_msgq.msg_dequeue(dp);

    int32_t data_serial = *(int32_t*) dp.pos_;
    int64_t file_offset = *(int64_t*) (dp.pos_+4);
    printf("recv:%d|%ld|%s", data_serial, file_offset, (dp.pos_ + 12));
    //  工作 写文件。。。
    free(dp.pos_);
    if (data_serial == -1 && file_offset == -1) break;
  }
  return NULL;
}


int main (int argc, char* argv[])
{

  struct timeval tv_begin, tv_end;
  gettimeofday(&tv_begin, NULL);

  if (argc != 5) {
    printf("%s pkg_cnt deal_thread_cnt sendrecv_thread_cnt zmq_io_thread_cnt\n", argv[0]);
    return 1;
  }

  pkg_top_cnt = atoi(argv[1]);
  int32_t deal_thread_cnt = atoi(argv[2]);
  int32_t sendrecv_thread_cnt = atoi(argv[3]);
  int32_t zmq_io_thread_cnt = atoi(argv[4]);

  // 启动zmq client
  void *context = zmq_init (zmq_io_thread_cnt);

  //  启动一个io worker池
  int thread_nbr = 0;
  pthread_t thread_ts[1024];
  for (thread_nbr = 0; thread_nbr < sendrecv_thread_cnt; thread_nbr++) {
    pthread_t worker;
    pthread_create (&worker, NULL, worker_routine, context);
    thread_ts[thread_nbr] = worker;
  }

  // 启用用户收发线程池
  for (thread_nbr = sendrecv_thread_cnt; thread_nbr < sendrecv_thread_cnt + deal_thread_cnt; thread_nbr++) {
    pthread_t worker;
    pthread_create (&worker, NULL, deal_routine, context);
    thread_ts[thread_nbr] = worker;
  }

  // 等待用户线程收发结束
  for (int t = sendrecv_thread_cnt; t < thread_nbr; t++) {
    pthread_join(thread_ts[t], NULL);
  }

  //  程序不会运行到这里，但仍进行清理工作
  zmq_term (context);

  gettimeofday(&tv_end, NULL);
  int32_t tc = tv_end.tv_sec - tv_begin.tv_sec + (tv_end.tv_usec - tv_begin.tv_usec) / 1000000;
  printf("pkg_cnt = %d, deal_thread_cnt = %d, sendrecv_thread_cnt = %d, zmq_io_thread_cnt = %d, time_cost = %ds",
         pkg_top_cnt, deal_thread_cnt, sendrecv_thread_cnt, zmq_io_thread_cnt, tc);
  return 0;
}
