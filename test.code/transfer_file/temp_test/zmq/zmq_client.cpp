//  Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main (void)
{
  //  Socket to talk to clients
  void *context = zmq_init(2);
  void *responder = zmq_socket (context, ZMQ_REQ);
  int rc = zmq_connect (responder, "tcp://10.97.184.60:5555");
  assert (rc == 0);

  while (1) {
    printf("say [Y/N]\n");
    char c;
    scanf("%c", &c);
    char buffer [256];buffer[0] = 'R';
    buffer[1] = '\0';
    rc = zmq_send (responder, buffer, 1, 0);
    printf ("send:%d, %s.\n", rc, buffer);
    int32_t part = 0;
    do {
      ++part;
      rc = zmq_recv(responder, buffer, sizeof(buffer), 0);
      buffer[rc] = '\0';
      printf ("recv:%d, %s.\n", part, buffer);
      int32_t more = 0;
      size_t more_size = sizeof (more);
      zmq_getsockopt (responder, ZMQ_RCVMORE, &more, &more_size);
      if (!more)
        break; // 已到达最后一帧
    } while(1);
    /*
    while (1) {
      zmq_msg_t message;
      zmq_msg_init (&message);
      rc = zmq_recvmsg(responder, &message, 0);
      printf ("recv:%d, %s.\n", rc, zmq_msg_data(&message));
      zmq_msg_close (&message);
      int32_t more = 0;
      size_t more_size = sizeof (more);
      zmq_getsockopt (responder, ZMQ_RCVMORE, &more, &more_size);
      if (!more)
        break; // 已到达最后一帧
    }
    */
  }
  zmq_close (responder);
  zmq_term (context);
  return 0;
}
