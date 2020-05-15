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

#include "mmap_mempool64.h"

class SocketBuff;
extern util::MMapMempool64 g_mmap_pool;

int32_t DeserializeBricks(char* brick_pkg, int32_t pkg_len);
int32_t handle_tcp_read(SocketBuff* socket_buff, char* brick_pkg, int32_t size);

void* PreLoadFile(void* file);
int32_t LoadResourceFile(char* file);

void AddOneClient(int32_t client_sock, int32_t client_num);
void* thread_epoll_worker_main(void* args);
void StartEpollWorker(int32_t epoll_num);

#endif  // HANDLE_H_
