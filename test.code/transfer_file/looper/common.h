#ifndef COMMOM_H
#define COMMON_H

#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include "Buffer.h"

void setNonBlocking(int fd){
  int optval=fcntl(fd,F_GETFL,0);
  fcntl(fd,F_SETFL,optval | O_NONBLOCK); 
  return ;
}

int listenOrDie(int port){
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  int flag = 1;
  int len = sizeof(flag);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, len); 

  if(sock == -1){
    std::cerr<<"create sock error : "<<strerror(errno)<<std::endl;
    exit(-1);
  }


  sockaddr_in sa;
  memset(&sa,0,sizeof(sa));
  sa.sin_port = htons(port);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);

  int ret = bind(sock, (sockaddr*)&sa, sizeof(sa));
  if(ret == -1){
    std::cerr<<"bind error : "<<strerror(errno)<<std::endl;
    exit(-1);
  }

  ret = listen(sock,512);
  if(ret == -1){
    std::cerr<<"listen error : "<< strerror(errno) << std::endl;
    exit(-1);
  }

  return sock;
}

int connectOrDie(std::string server, int port){
  int sock = socket(AF_INET,SOCK_STREAM, 0);
  if(sock == -1){
    std::cerr<<"create socket error " << strerror(errno) << std::endl;
    exit(1);
  }
  
  sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  inet_pton(AF_INET, server.c_str(), &sa.sin_addr);
  socklen_t len = sizeof(sa);
  
  int ret = connect(sock, (sockaddr *)&sa, len); 
  if(ret == -1){
    std::cout<<"connect error " << strerror(errno) << std::endl;
    exit(1);
  }
  
  return sock;
}

void printTime(){
  time_t now = time(NULL);
  tm * t = localtime(&now); 
  char buf[20];
  strftime(buf,20,"%Y-%m-%d %H:%M:%S",t);
  std::cout<<buf<<std::endl;
}
#endif
