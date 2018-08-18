#include "handle.h"
#include "common.h"
#include "socket_buff.h"
#include "epoll_worker.h"


EpollWorker g_epoll_worker[24];

int32_t g_epoll_worker_num = 1;
// 一块砖头编号的长度，默认是4个字节，表示一个int32_t
// TODO int is 4, int64 is 8 ?
const int32_t kBrickSerialLen = 4;
// 一块砖最大长度 256
const int32_t kMaxBrickLen = 256;


util::file_desc g_file_desc;
util::MMapMempool64 g_mmap_pool;

int32_t handle_brick_new2(char* recv_buf, int32_t begin, int32_t end, char* brick_pkg);

int32_t handle_tcp_read(SocketBuff* socket_buff, char* brick_pkg, int32_t size) {
  char* recv_buf;
  int32_t recv_size;
  int32_t pkg_len = 0;
  while(socket_buff->NextPacket(recv_buf, recv_size)) {
    int32_t req_num = (uint8_t)(recv_buf[0]);
    int32_t batchs = 50;
    int32_t begin = 1;
    int32_t end = batchs;
    do {
      if (begin > req_num) break;
      if (end > req_num) end = req_num;
      int32_t rc = handle_brick_new2(recv_buf, begin, end, brick_pkg + pkg_len);
      pkg_len += rc;
      begin += batchs;
      end += batchs;
    } while(true);
  }
  assert(pkg_len <= size);
  return pkg_len;

}

int32_t DeserializeBricks(char* brick_pkg, int32_t pkg_len) {
  int32_t cursor = sizeof(uint16_t);
  int32_t brick_pkg_len = *(uint16_t*) brick_pkg;
  int32_t pkg_cnt = (uint8_t) brick_pkg[cursor++];

  printf("brick_pkg_len:%d, pkg_cnt:%d\n", brick_pkg_len, pkg_cnt);
  for (int32_t i = 0; i < pkg_cnt; i++) {
    printf("channel_id:%d|", *(uint16_t*)(brick_pkg +cursor));
    cursor += sizeof(uint16_t);
    int32_t brick_len = (uint8_t)brick_pkg[cursor++];
    printf("brick_len:%d|", brick_len);
    printf("brick_serial:%d|", *(int32_t*)(brick_pkg + cursor));
    cursor += kBrickSerialLen;
    int32_t msg_len = brick_len - 1 - kBrickSerialLen;
    std::string msg(brick_pkg+cursor, msg_len);
    printf("brick_msg:%s|\n", msg.c_str());
    cursor += msg_len;
  }
  return 0;
}

int32_t fill_empty_brick(int32_t channel_id, const DataPos& dp, char* brick_pkg) {
//  pkg_len|pkg_cnt|channel_id|msg_len|brick_serial|msg|
  int32_t buff_num = 0;
  uint8_t brick_len = 1 + kBrickSerialLen;

  *(uint16_t*) (brick_pkg+buff_num) = (uint16_t)channel_id;
  buff_num += sizeof(uint16_t);

  // 记录处理后msg的长度
  brick_pkg[buff_num++] = (uint8_t) (brick_len);

  // 记录处理后msg的砖头序号
  *(uint32_t*) (brick_pkg + buff_num) = -1;
  buff_num += kBrickSerialLen;

  brick_pkg[buff_num] = '\0';

  return buff_num;
}

int32_t fill_one_brick(int32_t channel_id, const DataPos& dp, char* brick_pkg) {
//  pkg_cnt|channel_id|msg_len|brick_serial|msg|
  char* src = dp.pos_;
  int32_t src_len = dp.len_;
  int32_t buff_num = 0;
  int32_t begin = src_len / 3;
  int32_t mid = src_len / 3 + src_len / 3;
  uint32_t rowno = (uint32_t)dp.rowno_;

  uint8_t brick_len = src_len - begin + 1 + kBrickSerialLen;

  // 设置子包channel id
  *(uint16_t*) (brick_pkg+buff_num) = (uint16_t)channel_id;
  buff_num += sizeof(uint16_t);

  // 记录处理后msg的长度
  brick_pkg[buff_num++] = (uint8_t) (brick_len);

  // 记录处理后msg的砖头序号
  *(uint32_t*) (brick_pkg + buff_num) = rowno;
  buff_num += kBrickSerialLen;

  // 拷贝字符串
 // for (int32_t i = 0; i < src_len; i++) {
  for (int32_t i = src_len - 1; i >= 0; i--) {
    if (i >= mid || i < begin) {
      brick_pkg[buff_num++] = src[i];
    }
  }

  brick_pkg[buff_num] = '\0';
  
  return buff_num;
}

int32_t handle_brick_new2(char* recv_buf, int32_t begin, int32_t end, char* brick_pkg) {
    // 处理batch请求
  DataPos dp;

  int32_t pkg_len = 2;
  // 设置子包的个数
  brick_pkg[pkg_len] = (uint8_t) (end - begin + 1);
  ++pkg_len;
  int32_t j = 0;
  for (int32_t i = begin; i <= end; i++) {
    j = 2 * i - 1;
    uint16_t channel_id = *(uint16_t*) (recv_buf + j);
    bool is_no_data = false;
    uint32_t curline = 0;
    if (g_mmap_pool.data_queue_.dequeue(dp, curline) == 2) {
      usleep(5);
      while(g_mmap_pool.data_queue_.dequeue(curline, dp) == 2) {
        if (g_mmap_pool.data_queue_.is_over_) {
          //server 没有数据了
          is_no_data = true;
          break;
        }
        usleep(5);
      }
    }

    if (is_no_data) {
      int32_t rc = g_mmap_pool.data_queue_.dequeue(curline, dp);
      if (rc == 2 && g_mmap_pool.data_queue_.is_over_) {
        //server 没有数据了
        is_no_data = true;
      }
      else {
        is_no_data = false;
      }
    }


    if (is_no_data) {
      pkg_len += fill_empty_brick (channel_id, dp, brick_pkg + pkg_len);
    }
    else {
      pkg_len += fill_one_brick (channel_id, dp, brick_pkg + pkg_len);
    }
  }

  *(uint16_t*) brick_pkg = (uint16_t) pkg_len;
  //DeserializeBricks(brick_pkg, pkg_len);
  return pkg_len;
}


void* PreLoadFile(void* file) {
  if(g_mmap_pool.Init((char*)file) != 0) return NULL;

  g_file_desc.base = g_mmap_pool.get_base();
  g_file_desc.file_size = g_mmap_pool.get_file_size();
  util::MMapMempool64::PreLoadFile(&g_file_desc);
  return NULL;
}

int32_t LoadResourceFile(char* file) {
  pthread_t load_worker;
  pthread_create(&load_worker, NULL, PreLoadFile, file);

  return 0;
}

void AddOneClient(int32_t client_sock, int32_t client_num) {
  g_epoll_worker[client_num % g_epoll_worker_num].AddClient(client_sock);
}

void* thread_epoll_worker_main(void* args) {
  EpollWorker* epoll_worker = (EpollWorker*) args;
  epoll_worker->run();
  return NULL;
}

void StartEpollWorker(int32_t epoll_num) {
  g_epoll_worker_num = epoll_num;
  pthread_t con_thread_id;
  for (int32_t i = 0; i < epoll_num; i++) {
    if (pthread_create(&con_thread_id, NULL, thread_epoll_worker_main, &g_epoll_worker[i]) != 0) {
      perror("pthread_create() failed");
      exit(1);
    }
  }
}
