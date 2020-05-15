#include "handle.h"
#include "common.h"
#include "mmap_mempool64.h"
#include "req_queue.h"
#include "brick_queue.h"
#include "socket_buff.h"

// 一块砖头编号的长度，默认是4个字节，表示一个int32_t
// TODO int is 4, int64 is 8 ?
const int32_t kBrickSerialLen = 4;
// 一块砖最大长度 256
const int32_t kMaxBrickLen = 256;

// 全局链接个数
int32_t g_connect_number = 0;

// 全局处理数据线程数
int32_t g_deal_thread_number = 0;

util::file_desc g_file_desc;
util::MMapMempool64 g_mmap_pool;

// server最多有256个线程处理brick
ReqQueue g_req_queue[256];

// server最多有256个链接
BrickQueue g_brick_queue[256];

// read buff size
const int32_t kRecvBufSize = 8 * 1024;

void handle_tcp_read(void* thread_args) {
  int32_t  client_sock;
  int32_t  connect_number;
  client_sock = ((struct ThreadArgs*) thread_args)->client_sock_;
  connect_number = ((struct ThreadArgs*) thread_args)->connect_number_;
  free(thread_args);
  //TODO
  //1. recv
  //2. construct a req(channel_id, connect_number)
  //3. 计算当前应该放入那个线程req_queue cur_req_queue_id = req_num % g_deal_thread_number
  //2. 如果connect_number是奇数,放入该线程的req_queue,否则就放入
  //g_deal_thread_number-cur_req_queue_id-1里面
  //
  int32_t cur_req_queue_id = 0;
  int32_t channel_id = 0;
  int32_t req_num = 0;
  int64_t thread_id = pthread_self();
  SocketBuff* socket_buff = new(std::nothrow) SocketBuff(client_sock);
  Req req;
  req.connection_id_ = connect_number;

  DLOG("Thread %ld tcp read run\n", pthread_self());

  while(true) {
    char* recv_buf = NULL;
    int32_t recv_size = 0;
    while(socket_buff->NextPacket(recv_buf, recv_size)) {
      req_num = (uint8_t)(recv_buf[0]);
      for (int32_t r = 1; r <= req_num; r++) {
        channel_id = (uint8_t) (recv_buf[r]);
        req.channel_id_ = channel_id;
        if (connect_number % 2) {
          cur_req_queue_id = g_deal_thread_number - cur_req_queue_id- 1;
        }
        g_req_queue[cur_req_queue_id].enqueue(req);
        DLOG("thread %ld enqueue req, req_pkg_num = %d, [%d] req_queue_id = %d, req_channel_id = %d, req_connection_id = %d\n",
             thread_id,
             r,
             req_num,
             cur_req_queue_id,
             req.channel_id_,
             req.connection_id_);
        cur_req_queue_id = (cur_req_queue_id + 1) % g_deal_thread_number;
      }
    }
    DLOG("NextPacket Fail, Thread %ld\n", thread_id);
  }

  close(client_sock);
  return;

}

void FreeBricks(std::vector<Brick>& bricks) {
  for (typeof(bricks.begin()) it = bricks.begin();
       it != bricks.end();
       ++it) {
    free((*it).brick_);
  }
}

int32_t DeserializeBricks(char* brick_pkg, int32_t pkg_len) {
  int32_t cursor = sizeof(uint16_t);
  int32_t brick_pkg_len = *(uint16_t*) brick_pkg;
  int32_t pkg_cnt = (uint8_t) brick_pkg[cursor++];

  DLOG("brick_pkg_len:%d, pkg_cnt:%d\n", brick_pkg_len, pkg_cnt);
  for (int32_t i = 0; i < pkg_cnt; i++) {
    DLOG("channel_id:%d|", (uint8_t)brick_pkg[cursor++]);
    int32_t brick_len = (uint8_t)brick_pkg[cursor++];
    DLOG("brick_len:%d|", brick_len);
    DLOG("brick_serial:%d|", *(int32_t*)(brick_pkg + cursor));
    cursor += kBrickSerialLen;
    int32_t msg_len = brick_len - 1 - kBrickSerialLen;
    std::string msg(brick_pkg+cursor, msg_len);
    DLOG("brick_msg:%s|\n", msg.c_str());
    cursor += msg_len;
  }
  return 0;
}

int32_t SerializeBricks(char* brick_pkg,
                      std::vector<Brick>& bricks,
                      int32_t real_pkg_len) {
  // |char|char|char|4B|msg|
  // pkg_cnt|channel_id|msg_len|brick_serial|msg|
  // real_pkg_len 是所有砖头的长度之和
  int32_t cursor = sizeof(uint16_t);
  brick_pkg[cursor++] = (uint8_t) (bricks.size());
  for (typeof(bricks.begin()) it = bricks.begin();
       it != bricks.end();
       ++it) {
    brick_pkg[cursor++] = (uint8_t) ((*it).channel_id_);
    memcpy(brick_pkg+cursor, (*it).brick_, (*it).len_);
    cursor += (*it).len_;
  }
  brick_pkg[cursor] = '\0';

  uint16_t brick_pkg_len = (real_pkg_len /*所有砖头的长度之和*/ + 1/*每个砖头的channel_id*/ * bricks.size() + 1/*pkg_cnt*/ + sizeof(uint16_t));
  *(uint16_t*) brick_pkg = brick_pkg_len;
  assert(cursor == brick_pkg_len);

  return brick_pkg_len;
}

void handle_tcp_write(void* thread_args) {
  int32_t  client_sock;
  int32_t  connect_number;
  int32_t  brick_pkg_max_len;
  client_sock = ((struct ThreadArgs *) thread_args)->client_sock_;
  connect_number = ((struct ThreadArgs *) thread_args)->connect_number_;
  brick_pkg_max_len = ((struct ThreadArgs *) thread_args)->brick_pkg_max_len_;
  free(thread_args);

  char* brick_pkg = (char*) malloc(2 * brick_pkg_max_len);

  DLOG("Thread %ld tcp write run\n", pthread_self());
  std::vector<Brick> bricks;
  while (true) {
    int32_t pkg_len = 0;
    bricks.clear();
    g_brick_queue[connect_number].dequeue(bricks, brick_pkg_max_len, pkg_len);
    pkg_len = SerializeBricks(brick_pkg, bricks, pkg_len);

    DLOG("Thread %ld tcp write, get %lu bricks\n", pthread_self(), bricks.size());

    FreeBricks(bricks);

    // TODO 校验发送成功及连接是否ok
    int32_t send_len = 0;
    do {
      send_len = ::send(client_sock, brick_pkg, pkg_len, 0);
    } while(send_len <= 0);

    if (send_len != pkg_len) {
      DLOG("send error, brick_num:%lu, pkg_len:%d, send_len:%d, client_sock:%d, connect_number:%d\n",
           bricks.size(),
           pkg_len,
           send_len,
           client_sock,
           connect_number);
    } else {
      DLOG("send ok, brick_num:%lu, pkg_len:%d, send_len:%d, client_sock:%d, connect_number:%d\n",
           bricks.size(),
           pkg_len,
           send_len,
           client_sock,
           connect_number);
    }
    assert(send_len == pkg_len);
    // TODO DEBUG
    DeserializeBricks(brick_pkg, pkg_len);
  }

  free(brick_pkg);

  // TODO pkgman to send
  // 1. get bricks from g_brick_queue[connect_number]
  // 2. SerializeToString
  // 3. send to client_sock
  // 4. free the 
  // 5. user protobuf?

  // TODO 校验发送数据是否ok
}

char* deal_brick(int32_t& brick_len, DataPos& dp) {
  char* src = dp.pos_;
  int32_t src_len = dp.len_;
  int32_t buff_num = 0;
  int32_t begin = src_len / 3;
  int32_t mid = src_len / 3 + src_len / 3;
  uint32_t rowno = (uint32_t)dp.rowno_; //TODO int serial if int64 serial ?

  brick_len = src_len - begin + 1 + kBrickSerialLen;
  char* buffer = (char*) malloc(brick_len + 1);
  // 记录处理后msg的长度
  buffer[buff_num++] = (uint8_t) (brick_len);

  // 记录处理后msg的砖头序号
  *(uint32_t*) (buffer + buff_num) = rowno;
  buff_num += kBrickSerialLen;

  // 字符串倒序截取
  for (int32_t i = src_len - 1; i >= 0; i--) {
    if (i >= mid || i < begin) {
      buffer[buff_num++] = src[i];
    }
  }

  buffer[buff_num] = '\0';
  
  return buffer;
}

void handle_brick(void* thread_args) {
  int32_t req_queue_id = *(int32_t*)thread_args;
  DLOG("Thread %ld handle brick run, req_queue_id = %d\n", pthread_self(), req_queue_id);
  while(true) {
    // TODO handle g_req_queue[req_queue_id]
    // 1. get req from g_req_queue[req_queue_id]
    // 2. get brick from data_queue
    // 3. deal_brick
    // 4. enqueue it to g_brick_queue[req->connection_number_]
    Req req;
    g_req_queue[req_queue_id].dequeue(req);

    DLOG("Thread %ld deal brick, req_queue_id %d, req_channel_id %d, req_connect_id %d\n",
           pthread_self(),
           req_queue_id,
           req.channel_id_,
           req.connection_id_);
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
      DLOG("data queue dequeue error.\n");
    }

    Brick brick;
    if (is_no_data) {
      int32_t brick_len = 1 + kBrickSerialLen;
      char* buffer = (char*) malloc(brick_len + 1);
      buffer[0] = (uint8_t) (brick_len);
      // TODO, 砖头序号用64位？？
      *(uint32_t*) (buffer + 1) = -1;
      buffer[brick_len] = '\0';
      brick.channel_id_ = req.channel_id_;
      brick.brick_ = buffer;
      brick.len_ = brick_len;
      g_brick_queue[req.connection_id_].enqueue(brick);
      DLOG("enqueue brick queue: #%d|%d|%s#\n", *(uint8_t*)buffer, *(int32_t*) (buffer+1), buffer+5);
    }
    else {
      int32_t brick_len = dp.len_;
      // TODO 实现deal_brick
      char* buffer = deal_brick(brick_len, dp);
      brick.channel_id_ = req.channel_id_;
      brick.brick_ = buffer;
      brick.len_ = brick_len;
      assert(brick_len > 4);
      g_brick_queue[req.connection_id_].enqueue(brick);
      DLOG("enqueue brick queue: #%d|%d|%s#\n", *(uint8_t*)buffer, *(int32_t*) (buffer+1), buffer+5);
    }
  }
}


void *thread_io_read_main(void* thread_args) {
  /* Guarantees that thread resources are deallocated upon return */
  pthread_detach(pthread_self()); 

  /* Deallocate memory for argument */
  // free(thread_args);

  handle_tcp_read(thread_args);

  return (NULL);
}

void *thread_io_write_main(void* thread_args) {
  /* Guarantees that thread resources are deallocated upon return */
  pthread_detach(pthread_self()); 

  /* Deallocate memory for argument */
  // free(thread_args);

  handle_tcp_write(thread_args);

  return (NULL);
}

void *thread_dealbrick_main(void* thread_args) {
  /* Guarantees that thread resources are deallocated upon return */
  pthread_detach(pthread_self()); 

  /* Deallocate memory for argument */
  // free(thread_args);

  handle_brick(thread_args);

  return (NULL);
}

int32_t LoadResourceFile(char* file) {
  if(g_mmap_pool.Init(file) != 0) return 1;

  pthread_t load_worker;
  g_file_desc.base = g_mmap_pool.get_base();
  g_file_desc.file_size = g_mmap_pool.get_file_size();
  pthread_create(&load_worker, NULL, util::MMapMempool64::PreLoadFile, &g_file_desc);

  return 0;
}

int32_t CreateDealThreadPool(int32_t deal_thread_num) {
  g_deal_thread_number = deal_thread_num;
  for (int32_t i = 0; i < deal_thread_num; i++) {
    pthread_t deal_worker;
    int32_t* req = (int32_t*) malloc(sizeof(int32_t));
    *req = i;
    pthread_create(&deal_worker, NULL, thread_dealbrick_main, req);
  }
  return 0;
}

int32_t CreateIoThreadPool(int32_t client_sock,
                           int32_t brick_pkg_max_len) {
  struct ThreadArgs *thread_args;

  /* Create separate memory for client argument */
  thread_args = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
  thread_args->client_sock_ = client_sock;
  thread_args->connect_number_ = g_connect_number;
  thread_args->brick_pkg_max_len_ = brick_pkg_max_len;

  /* Create thread to read the connect*/
  pthread_t con_thread_id;
  if (pthread_create(&con_thread_id, NULL, thread_io_read_main, (void *)thread_args) != 0) {
    perror("pthread_create() failed");
    exit(1);
  }

  thread_args = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
  thread_args->client_sock_ = client_sock;
  thread_args->connect_number_ = g_connect_number;
  thread_args->brick_pkg_max_len_ = brick_pkg_max_len;

  /* Create thread to write the connect*/
  if (pthread_create(&con_thread_id, NULL, thread_io_write_main, (void *)thread_args) != 0) {
    perror("pthread_create() failed");
    exit(1);
  }

  DLOG("with thread %ld toread socket %d, connect_number %d\n", (int64_t)con_thread_id, client_sock, g_connect_number);
  ++g_connect_number;

  return 0;
}
