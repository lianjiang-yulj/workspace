//  Hello World server

#include <zmq.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main (void)
{
  //  Socket to talk to clients
  void *context = zmq_ctx_new ();
  void *responder = zmq_socket (context, ZMQ_REP);
  int rc = zmq_bind (responder, "tcp://10.97.184.60:5555");
  assert (rc == 0);

  zmq_msg_t message;
  while (1) {
    do {
      zmq_msg_init (&message);
      zmq_recvmsg (responder, &message, 0);
      int size = zmq_msg_size (&message);
      assert(size == 14);
      char *string = (char*) malloc (size + 1);
      memcpy (string, zmq_msg_data (&message), size);
      string [size] = 0;

      int head1 = *(int*)string;
      int64_t head2 = *(int64_t*) (string+4);
      printf("len = %d, head1 =%d, head2 = %ld, left = %s\n", size, head1, head2, string+4+8);
      /*
      int32_t more = 0;
      size_t more_size = sizeof (more);
      zmq_getsockopt (responder, ZMQ_RCVMORE, &more, &more_size);                                                                                              
      if (!more)
        break; // 已到达最后一帧
      */
      bool bmore = zmq_msg_more(&message);
      if (!bmore) {
        break;
      }
    } while(true);

    zmq_msg_close (&message);
    printf("ok\n");

  }
  zmq_close (responder);
  zmq_term (context);
  return 0;
}
