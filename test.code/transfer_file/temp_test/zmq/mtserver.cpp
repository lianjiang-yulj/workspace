//
//  多线程版Hello World服务
//
//#include "zhelpers.h"
#include <zmq.h>
#include <malloc.h>
#include <string>
#include <assert.h>
#include <pthread.h>

const int bs = 1024 * 1024 + 1;
char buffer[bs];
int byout = 0;
int send_more = 0;

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

static void *
worker_routine (void *context) {
  //  连接至代理的套接字
  void *receiver = zmq_socket (context, ZMQ_REP);
  int rc = zmq_connect (receiver, "inproc://workers");
  assert(rc == 0);

  while (1) {
    char *string = s_recv (receiver);
    int len = strlen(string);
    assert(len == 1);
    //   printf ("Received request: [%s]\n", string);
    free (string);
    //  工作
    //sleep (1);
    //  返回应答
    s_send (receiver, buffer);
  }
  zmq_close (receiver);
  return NULL;
}

int main (int argc, char* argv[])
{
  if (argc != 5) {
    printf("%s bytes deal_thread_cnt io_thread_cnt send_more\n", argv[0]);
    return 1;
  }

  memset(buffer, 'R', sizeof(buffer));
  byout = atoi(argv[1]);
  send_more = atoi(argv[4]);
  buffer[byout] = '\0';
  void *context = zmq_init (atoi(argv[3]));

  //  用于和client进行通信的套接字
  void *clients = zmq_socket (context, ZMQ_ROUTER);
  zmq_bind (clients, "tcp://10.97.184.60:5555");

  //  用于和worker进行通信的套接字
  void *workers = zmq_socket (context, ZMQ_DEALER);
  zmq_bind (workers, "inproc://workers");

  //  启动一个worker池
  int thread_nbr;
  for (thread_nbr = 0; thread_nbr < atoi(argv[2]); thread_nbr++) {
    pthread_t worker;
    pthread_create (&worker, NULL, worker_routine, context);
  }
  //  启动队列装置
  zmq_device (ZMQ_QUEUE, clients, workers);

  //  程序不会运行到这里，但仍进行清理工作
  zmq_close (clients);
  zmq_close (workers);
  zmq_term (context);
  return 0;
}
