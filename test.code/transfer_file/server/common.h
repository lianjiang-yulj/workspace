#ifndef COMMON_H__
#define COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>

int32_t setnonblock(int32_t fd);
int32_t setreuseaddr(int32_t fd);
void die_with_message(char* msg);
int32_t make_socket_non_blocking (int32_t fd);
int32_t make_socket_blocking (int32_t fd);
int32_t set_socket_keepalive(int32_t fd, int32_t keep_alive);
int32_t set_socket_linger(int32_t fd, int32_t linger);
int32_t set_socket_recv_buffer(int32_t fd, int32_t size);
int32_t set_socket_send_buffer(int32_t fd, int32_t size);
int32_t set_socket_max_buffer(int32_t fd);
int32_t set_socket_nodelay(int32_t fd, int32_t no_delay);
std::string address_to_string(struct sockaddr_in* addr);
int32_t create_tcp_server_socket (char* port);
int32_t accept_tcp_connection(int32_t server_sock);

#define DLOG(fmt, args...) \
    printf(fmt, ##args)


#endif // COMMON_H__

