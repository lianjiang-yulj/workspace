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
#include "atomic.h"

const int bs = 1024 * 1024 + 1;
char buffer[bs];
int byin = 0;
int pkg_top_cnt = 0;
int send_more = 0;

atomic_t recv_total_cnt;

//  从套接字中获取ZMQ字符串，并转换为C语言字符串
static char *
s_recv (void *socket);

// 从套接字后去ZMQ 多帧消息
static char*
s_more_recv(void *socket) {
  int32_t part = 0;
  char *ret_string = (char*) malloc (send_more * byin + 1);
  do {
    ++part;
    char* string = s_recv(socket);
    int len = strlen(string);
    assert(len == byin);
    memcpy(ret_string+(part-1)*byin, string, len);
    int32_t more = 0;
    size_t more_size = sizeof (more);
    zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &more_size);
    if (!more)
      break; // 已到达最后一帧
  } while(1);
  assert(part == send_more);
  ret_string[send_more * byin] = '\0';
  return ret_string;
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
s_send (void *socket, char *string) {
  int rc;
  zmq_msg_t message;
  zmq_msg_init_size (&message, strlen (string));
  memcpy (zmq_msg_data (&message), string, strlen (string));
  rc = zmq_sendmsg (socket, &message, 0);
  assert (rc == strlen(string));
  zmq_msg_close (&message);
  return (rc);
}

static void *
worker_routine (void *context) {
  //  连接至代理的套接字
  void *receiver = zmq_socket (context, ZMQ_REQ);
  int rc = zmq_connect (receiver, "tcp://10.97.184.60:5555");
  assert(rc == 0);

  while (1) {
    // 发送请求
    s_send (receiver, buffer);
    char *string = s_more_recv (receiver);
    //char *string = s_recv (receiver);
    //   printf ("Received request: [%s]\n", string);
    //  工作
    //sleep (1);
    int len = strlen(string);
    assert(len == byin * send_more);
    free (string);
    len = atomic_add_return(send_more, &recv_total_cnt);
    if (len >= pkg_top_cnt) break;
  }
  zmq_close (receiver);
  return NULL;
}

int main (int argc, char* argv[])
{

  struct timeval tv_begin, tv_end;
  gettimeofday(&tv_begin, NULL);

  atomic_set(&recv_total_cnt, 0);
  if (argc != 6) {
    printf("%s bytes deal_thread_cnt io_thread_cnt recv_pkg_cnt send_more\n", argv[0]);
    return 1;
  }

  byin = atoi(argv[1]);
  pkg_top_cnt = atoi(argv[4]);
  send_more = atoi(argv[5]);

  memset(buffer, 'R', sizeof(buffer));
  buffer[1] = '\0';
  void *context = zmq_init (atoi(argv[3]));

  //  启动一个worker池
  int thread_nbr;
  pthread_t thread_ts[1024];
  for (thread_nbr = 0; thread_nbr < atoi(argv[2]); thread_nbr++) {
    pthread_t worker;
    pthread_create (&worker, NULL, worker_routine, context);
    thread_ts[thread_nbr] = worker;
  }

  for (int t = 0; t < thread_nbr; t++) {
    pthread_join(thread_ts[t], NULL);
  }

  //  程序不会运行到这里，但仍进行清理工作
  zmq_term (context);

  gettimeofday(&tv_end, NULL);
  int32_t tc = tv_end.tv_sec - tv_begin.tv_sec + (tv_end.tv_usec - tv_begin.tv_usec) / 1000000;
  printf("byin = %dB, recv_total_cnt = %d, pkg_top_cnt = %d, send_more = %d, time_cost = %ds",
         byin, recv_total_cnt.counter, pkg_top_cnt, send_more, tc);
  return 0;
}
