#include <signal.h>

#include "common.h"
#include "handle.h"


int32_t main (int32_t argc, char *argv[]) {
  int32_t server_sock, client_sock;

  if (argc != 4) {
    printf("Usage:\n   %s [port] [data file] [io_num]\n", argv[0]);
    exit(1);
  }

  signal(SIGPIPE, SIG_IGN);

  char* port = argv[1];
  char* file_name = argv[2];

  // load file async
  LoadResourceFile(file_name);

  // start server
  server_sock = create_tcp_server_socket(port);
  if (server_sock == -1) {
    printf("create tcp server failed.\n");
    exit(1);
  }

  // start epoll async
  StartEpollWorker(atoi(argv[3]));

  int32_t client_num = 0;
  while (true) {
    // start one client connection
    client_sock = accept_tcp_connection(server_sock, client_num);
    AddOneClient(client_sock, client_num);
    ++client_num;
   //   CreateIoThreadPool(client_sock, brick_pkg_max_len);
  }

  close (server_sock);

  return 0;
}

