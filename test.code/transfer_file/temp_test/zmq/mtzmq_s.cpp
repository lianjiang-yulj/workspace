//
//  多线程版Hello World服务
//
//#include "zhelpers.h"
#include <zmq.h>
#include <malloc.h>
#include <string>
#include <assert.h>
#include <pthread.h>

#include <vector>
#include <list>
#include "Condition.h"
#include "../MMapMempool64.h"

int send_more = 0;
char* g_file_name;
util::file_desc g_file_desc;
util::MMapMempool64 g_mmap_pool;

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
s_send (void *socket, char *string) {
  int rc;
  for (int part = 0; part < send_more; part++) {
    zmq_msg_t message;
    zmq_msg_init_size (&message, strlen (string));
    memcpy (zmq_msg_data (&message), string, strlen (string));
    int flag = ZMQ_SNDMORE;
    if (part == send_more - 1) {
      flag = 0;
    }
    rc = zmq_sendmsg (socket, &message, flag);
    assert (rc == strlen(string));
    zmq_msg_close (&message);
  }
  return (rc);
}

//  将C语言字符串转换为ZMQ字符串，并发送给套接字
static int
s_more_send (void *socket, std::vector<DataPos>& msg) {
  int rc;
  for (int part = 0; part < msg.size(); part++) {
    zmq_msg_t message;
    zmq_msg_init_size (&message, msg[part].len_);
    memcpy (zmq_msg_data (&message), msg[part].pos_, msg[part].len_);
    int flag = ZMQ_SNDMORE;
    if (part == msg.size() - 1) {
      flag = 0;
    }
    rc = zmq_sendmsg (socket, &message, flag);
    assert (rc == msg[part].len_);
    zmq_msg_close (&message);
  }

  return msg.size();
}


static void *
worker_routine (void *context) {
  //  连接至代理的套接字
  void *receiver = zmq_socket (context, ZMQ_REP);
  int rc = zmq_connect (receiver, "inproc://workers");
  assert(rc == 0);

  std::vector<DataPos> msg;

  while (1) {
    char *string = s_recv (receiver);
    int req_num = strlen(string);
    // 长度代表请求个数
    assert(req_num > 0);
    //   printf ("Received request: [%s]\n", string);
    free (string);
    g_msgq.req_enqueue(req_num);
    //  工作
    msg.clear();
    g_msgq.msg_dequeue(msg, req_num);
    //  返回应答
    rc = s_more_send (receiver, msg);

    for (int i = 0; i < msg.size(); i++) {
      printf("send:%d|%ld|%s", *(int32_t*)msg[i].pos_, *(int64_t*)(msg[i].pos_+4), (msg[i].pos_ + 12));
      free(msg[i].pos_);
    }

    assert(rc == req_num);
  }
  zmq_close (receiver);
  return NULL;
}

int64_t file_offset = 0;
int64_t data_serial = -1;
int32_t deal_data(unsigned char* buffer, DataPos& dp) {
  char* src = dp.pos_;
  int32_t src_len = dp.len_;
  int32_t buff_num = 0;
  int32_t begin = src_len / 3;
  int32_t mid = src_len / 3 + src_len / 3;
  uint32_t rowno = (uint32_t)dp.rowno_; //TODO int serial if int64 serial ?
  int32_t head_len = 4;//TODO int is 4, int64 is 8 ?
  int32_t tail_len = 8;

  /*
  for (int j = 0; j < head_len; j++) {
    buffer[buff_num++] = (unsigned char)(rowno&0xff);
    rowno >>= 8;
  }
  */
  *(uint32_t*)buffer = rowno;
  buff_num += head_len;
  buff_num += tail_len;

  for (int32_t i = src_len - 1; i >= 0; i--) {
    if (i >= mid || i < begin) {
      buffer[buff_num++] = src[i];
    }
  }
  //TODO add tail offset of file
  //while (dp.rowno_ != data_serial + 1) {
   // usleep(1);
  //}
 
  int64_t temp_file_offset = file_offset;
  /*
  int32_t tail_len = 8;
  for (int j = 0; j < tail_len; j++) {
    buffer[buff_num++] = (unsigned char)(temp_file_offset&0xff);
    temp_file_offset >>= 8;
  }
  */
  *(uint64_t*)(buffer + head_len) = temp_file_offset;
  buffer[buff_num++] = '\r';
  buffer[buff_num++] = '\n';

  int32_t serial_len = 0;
  rowno = (int32_t)dp.rowno_;
  do {
    serial_len++;
    rowno /= 10;

  }while(rowno > 0);

  file_offset += serial_len; // TOOD cal file_offset
  file_offset += (buff_num - head_len - tail_len);

  data_serial ++; //TODO atomic
  buffer[buff_num] = '\0';
  return buff_num;
}

static void *
deal_routine (void *context) {
  while (true) {
    g_msgq.req_dequeue(1);
    char* buffer = (char*) malloc(256);
    // 处理一个请求
    DataPos dp;
    bool is_no_data = false;
    while(g_mmap_pool.data_queue_.dequeue(dp) == 2) {
      if (g_mmap_pool.data_queue_.is_over_) {
        //server 没有数据了
        is_no_data = true;
        break;
      }
      usleep(10);
    }

    if (is_no_data) {
      *(uint32_t*)buffer = -1;
      *(uint64_t*)(buffer+4) = -1;
      buffer[12] = '\r';
      buffer[13] = '\n';
      buffer[14] = '\0';
      dp.pos_ = buffer;
      dp.len_ = 14;
      g_msgq.msg_enqueue(dp);
      //printf("send: #%d|%s|%ld#\n", *(int32_t*)buffer, buffer+12, *(int64_t*)(buffer+4));
    }
    else {
      int32_t send_len = dp.len_;
      send_len = deal_data((unsigned char*)buffer, dp);
      dp.pos_ = buffer;
      dp.len_ = send_len;
      g_msgq.msg_enqueue(dp);
      //printf("send: #%d|%s|%ld#\n", *(int32_t*)buffer, buffer+12, *(int64_t*)(buffer+4));
    }
  }

  return NULL;
}



int LoadMapFile(char* file) {
  if(g_mmap_pool.Init(file) != 0) return 1;

  pthread_t load_worker;
  g_file_desc.base = g_mmap_pool.get_base();
  g_file_desc.file_size = g_mmap_pool.get_file_size();
  pthread_create (&load_worker, NULL, util::MMapMempool64::PreLoadFile, &g_file_desc);

  return 0;
}

int main (int argc, char* argv[])
{
  if (argc != 5) {
    printf("%s file deal_thread_cnt sendrecv_thread_cnt zmq_io_thread_cnt\n", argv[0]);
    return 1;
  }

  g_file_name = argv[1];
  int32_t deal_thread_cnt = atoi(argv[2]);
  int32_t sendrecv_thread_cnt = atoi(argv[3]);
  int32_t zmq_io_thread_cnt = atoi(argv[4]);

  // load file
  int rc = LoadMapFile(g_file_name);
  if (rc != 0) {
    printf("load file: %s error.\n", g_file_name);
    return rc;
  }

  //  启动一个deal worker池
  int thread_nbr = 0;
  for (thread_nbr = 0; thread_nbr < deal_thread_cnt; thread_nbr++) {
    pthread_t worker;
    pthread_create (&worker, NULL, deal_routine, NULL);
  }

  // init zmq server
  void *context = zmq_init (zmq_io_thread_cnt);

  //  用于和client进行通信的套接字
  void *clients = zmq_socket (context, ZMQ_ROUTER);
  zmq_bind (clients, "tcp://10.97.184.60:5555");

  //  用于和worker进行通信的套接字
  void *workers = zmq_socket (context, ZMQ_DEALER);
  zmq_bind (workers, "inproc://workers");

  //  启动一个send recv worker池
  thread_nbr = 0;
  for (thread_nbr = 0; thread_nbr < sendrecv_thread_cnt; thread_nbr++) {
    pthread_t worker;
    pthread_create (&worker, NULL, worker_routine, context);
  }
  //  启动队列装置
  zmq_device (ZMQ_QUEUE, clients, workers);

  // other wait all threads
  //  程序不会运行到这里，但仍进行清理工作
  zmq_close (clients);
  zmq_close (workers);
  zmq_term (context);
  return 0;
}
