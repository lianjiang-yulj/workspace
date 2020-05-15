#include "common.h"

int32_t setnonblock(int32_t fd){
  fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) | O_NONBLOCK);
  return 0;
}

int32_t setreuseaddr(int32_t fd){
  int32_t ok=1;
  setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&ok,sizeof(ok));
  return 0;
}

void die_with_message(char* msg) {
    perror(msg);
    exit(-1);
}


int32_t make_socket(int32_t fd, char blocking) {
  int32_t flags, s;

  flags = fcntl (fd, F_GETFL, 0);
  if (flags == -1) {
    perror ("fcntl");
    return -1;
  }

  if (blocking) {
    flags &= (~O_NONBLOCK);
  }else{
    flags |= O_NONBLOCK;
  }

  s = fcntl (fd, F_SETFL, flags);
  if (s == -1) {
    perror ("fcntl");
    return -1;
  }

  return 0;

}

int32_t make_socket_non_blocking (int32_t fd) {
  return make_socket(fd, 0);
}

int32_t make_socket_blocking (int32_t fd) {
  return make_socket(fd, 1);
}

#define OPTION_ENABLE   1
#define OPTION_DISABLE  2

int32_t set_socket_keepalive(int32_t fd, int32_t keep_alive) {
  int32_t opt_val = keep_alive ? OPTION_ENABLE : OPTION_DISABLE;
  socklen_t opt_len = sizeof(int32_t);

  if ( setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt_val, opt_len) != 0 ) {
    perror("set SO_KEEPALIVE");
    return -1;
  }

  return 0;
}

int32_t set_socket_linger(int32_t fd, int32_t linger) {
  struct linger lg;
  if (linger) {
    lg.l_onoff = OPTION_ENABLE;
    lg.l_linger = 0;
  } else {
    lg.l_onoff = OPTION_DISABLE;
  }

  if ( setsockopt(fd, SOL_SOCKET, SO_LINGER, &lg, sizeof(struct linger)) != 0 ) {
    perror("set SO_LINGER");
    return -1;
  }
  return 0;
}

int32_t set_socket_recv_buffer(int32_t fd, int32_t size) {
  if ( setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int32_t)) != 0 ) {
    perror("set SO_RCVBUF");
    return -1;
  }
  return 0;
}

int32_t set_socket_send_buffer(int32_t fd, int32_t size) {
  if ( setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(int32_t)) != 0 ) {
    perror("set SO_SNDBUF");
    return -1;
  }
  return 0;
}

#define MAXSNDBUF 16777216
#define MAXRCVBUF 16777216
int32_t set_socket_max_buffer(int32_t fd) {
  if (set_socket_send_buffer(fd, MAXSNDBUF)<-1 || set_socket_recv_buffer(fd, MAXRCVBUF)<0) {
    return -1;
  }
  return 0;
}

int32_t set_socket_nodelay(int32_t fd, int32_t no_delay) {
  int32_t opt_val = no_delay ? OPTION_ENABLE : OPTION_DISABLE;
  if ( setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(int32_t)) != 0 ) {
    perror("set TCP_NODELAY"); 
    return -1;
  }
  return 0;
}

std::string address_to_string(struct sockaddr_in* addr){
  char ip[128];
  inet_ntop(AF_INET,&(addr->sin_addr),ip,sizeof(ip));
  char port[32];
  snprintf(port,sizeof(port),"%d",ntohs(addr->sin_port));
  std::string r;
  r=r+"("+ip+":"+port+")";
  return r;
}

int32_t create_tcp_server_socket (char *port) {
  struct addrinfo hint32_ts;
  struct addrinfo *result, *rp;
  int32_t s, server_sock;

  memset (&hint32_ts, 0, sizeof (struct addrinfo));
  hint32_ts.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
  hint32_ts.ai_socktype = SOCK_STREAM; /* TCP socket */
  hint32_ts.ai_flags = AI_PASSIVE;     /* All int32_terfaces */

  s = getaddrinfo (NULL, port, &hint32_ts, &result);
  if (s != 0) {
    fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
    return -1;
  }

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    server_sock = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (server_sock == -1) {
      continue;
    }

    s = bind (server_sock, rp->ai_addr, rp->ai_addrlen);
    if (s == 0) {
      /* We managed to bind successfully! */
      break;
    }

    close (server_sock);
  }

  if (rp == NULL) {
    fprintf (stderr, "Could not bind\n");
    return -1;
  }

  printf("%d socket max connections\n", SOMAXCONN);
  //s = listen (server_sock, SOMAXCONN);
  s = listen (server_sock, 1024);
  if (s == -1) {
    perror ("listen");
    return -1;
  }

  freeaddrinfo (result);

 // setnonblock(server_sock);
  setreuseaddr(server_sock);
  return server_sock;
}

int32_t accept_tcp_connection(int32_t server_sock, int32_t& client_num) {
  int32_t client_sock;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  /* Wait for a client to connect */
  if ((client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addr_len)) < 0) {
    perror("accept() failed");
    return -1;
  }

  /* client_sock is connected to a client! */

  printf("client_num:%d, new client %s\n", client_num, address_to_string(&client_addr).c_str());
  client_num++;

  return client_sock;
}

