//  Hello World server

#include <zmq.h>
#include <stdio.h>
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

  while (1) {
 //   printf("say [Y/N]\n");
    char c;
 //   scanf("%c", &c);
    char buffer [43];
    rc = zmq_recv (responder, buffer, sizeof(buffer), 0);
    buffer[rc] = '\0';
    //printf ("recv:%d, %s.\n", rc, buffer);
    memset(buffer, 'z', sizeof(buffer));
    rc = zmq_send (responder, buffer, sizeof(buffer), ZMQ_SNDMORE);
   // printf ("send:%d, %s.\n", rc, buffer);
    memset(buffer, 'm', sizeof(buffer));
    rc = zmq_send (responder, buffer, sizeof(buffer), ZMQ_SNDMORE);
   // printf ("send:%d, %s.\n", rc, buffer);
    memset(buffer, 'q', sizeof(buffer));
    rc = zmq_send (responder, buffer, sizeof(buffer), 0);
   // printf ("send:%d, %s.\n", rc, buffer);
  }
  zmq_close (responder);
  zmq_term (context);
  return 0;
}
