//  Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

int main (void)
{
  //  Socket to talk to clients
  void *context = zmq_ctx_new ();
  void *responder = zmq_socket (context, ZMQ_REQ);
  int rc = zmq_connect (responder, "tcp://10.97.184.60:5555");
  assert (rc == 0);

  while (1) {
    printf("say [Y/N]\n");
    char c;
    scanf("%c", &c);
    char buffer [43];

      /*
      zmq_msg_t message;
      zmq_msg_init (&message);
      zmq_recvmsg (responder, &message, 0);
      int size = zmq_msg_size (&message);
      char *string = (char*) malloc (size + 1);
      memcpy (string, zmq_msg_data (&message), size);
      zmq_msg_close (&message);
      string [size] = 0;

      head1 = *(int*)string;
      head2 = *(int64_t*) (string+4);
      printf("len = %d, head1 =%d, head2 = %ld, left = %s\n", size, head1, head2, string+4+8);
      int32_t more = 0;
      size_t more_size = sizeof (more);
      zmq_getsockopt (responder, ZMQ_RCVMORE, &more, &more_size);                                                                                              
      if (!more)
        break; // 已到达最后一帧
        */
    int rc;
    *(int*) buffer = 1010;
    *(int64_t*)(buffer + 4) = 10000;
    buffer[12] = 'A';
    buffer[13] = 'B';
    buffer[14] = '\0';

    for (int part = 0; part < 3; part++) {
      zmq_msg_t message;
      zmq_msg_init_size (&message, 14);
      memcpy (zmq_msg_data (&message), buffer, 14);
      int flag = ZMQ_SNDMORE;
      if (part == 2) {
        flag = 0;
      }
      rc = zmq_sendmsg (responder, &message, flag);
      assert (rc == 14);
      zmq_msg_close (&message);
    }

  }
  zmq_close (responder);
  zmq_term (context);
  return 0;
}
