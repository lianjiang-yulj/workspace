#include "common.h"
#include "handle.h"

const int32_t kBrickPkgMaxLen = 8 * 1024;

int32_t main (int32_t argc, char *argv[]) {
  int32_t server_sock, client_sock;

  if (argc != 5) {
    printf("Usage:\n   %s [port] [data file] [deal_thread_cnt] [brick_pkg_len]\n", argv[0]);
    exit(1);
  }

  char* port = argv[1];
  char* file_name = argv[2];
  int32_t deal_thread_cnt = atoi(argv[3]);
  int32_t brick_pkg_max_len = atoi(argv[3]);

  if (brick_pkg_max_len < kBrickPkgMaxLen) {
    brick_pkg_max_len = kBrickPkgMaxLen;
  }

  // load file
  LoadResourceFile(file_name);

  // start deal thread pool
  CreateDealThreadPool(deal_thread_cnt);

  // start server
  server_sock = create_tcp_server_socket(port);
  if (server_sock == -1) {
    exit(1);
  }

  while (true) {
    // start one client connection
    client_sock = accept_tcp_connection(server_sock);
    CreateIoThreadPool(client_sock, brick_pkg_max_len);
  }

  close (server_sock);

  return 0;
}

