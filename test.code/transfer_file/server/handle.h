#ifndef HANDLE_H_
#define HANDLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <vector>

#include "brick_queue.h"

extern int32_t g_connect_number;  
extern int32_t g_deal_thread_number;  

void FreeBricks(std::vector<Brick>& bricks);
int32_t DeserializeBricks(char* brick_pkg, int32_t pkg_len);
int32_t SerializeBricks(char* brick_pkg,
                        std::vector<Brick>& bricks,
                        int32_t real_pkg_len);
void handle_tcp_read(void* thread_args);
void handle_tcp_write(void* thread_args);

struct ThreadArgs {
  int32_t client_sock_; /* Socket descriptor for client */
  int32_t connect_number_; /* connection serial*/
  int32_t brick_pkg_max_len_; /* max len of the brick pkg*/
};

void* thread_io_read_main(void* thread_args);
void* thread_io_write_main(void* thread_args);
void* thread_dealbrick_main(void* thread_args);

int32_t LoadResourceFile(char* file);
int32_t CreateDealThreadPool(int32_t deal_thead_num);
int32_t CreateIoThreadPool(int32_t client_sock,
                           int32_t brick_pkg_max_len);

#endif  // HANDLE_H_
