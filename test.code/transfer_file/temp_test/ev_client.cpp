#include <stdio.h>  
#include <netinet/in.h>  
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h> 
#include <sys/epoll.h>
#include <sys/time.h>
#include <ev.h>
#include <iostream>
#include <fcntl.h>
#include <getopt.h>
#include <sys/socket.h>   
#include <malloc.h>
#include <cstddef>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])   
{   
  int client_sockfd;   
  int len;   
  struct sockaddr_in remote_addr; 
  unsigned char buf[BUFFER_SIZE]; 
  memset(&remote_addr,0,sizeof(remote_addr)); 
  remote_addr.sin_family=AF_INET; 
  remote_addr.sin_addr.s_addr=inet_addr("10.97.184.60");
  remote_addr.sin_port=htons(9999); 

  if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)   
  {   
    perror("client socket creation failed");   
    exit(EXIT_FAILURE);
  }   

  if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)   
  {   
    perror("connect to server failed");   
    exit(EXIT_FAILURE);
  }  


  struct timeval tv_begin, tv_end;
  gettimeofday(&tv_begin, NULL);
  while(1)
  {
  //  printf("\nPlease input the message:");
 //   scanf("%s",buf);
    strcpy((char*)buf, "A");
    //printf("req:#%s#\n", buf);
    if(send(client_sockfd,buf,1,0) < 0) {
      printf("--send error--\n");
      continue;
    }

    while((len=recv(client_sockfd,buf,BUFFER_SIZE,0)) < 0) {
      printf("--recv error--\n");
      continue;
    }
    if (buf[len-1] != '\n' && buf[len-2] != '\r') {
      printf("--recv not ok--\n");
    }
    buf[len] = '\0';
    if (*(int32_t*)buf == -1 && *(int64_t*)(buf+4) == -1) {
      printf("receive from server:#%d|%s|%ld#\n",*(int32_t*)buf, buf+12, *(int64_t*)(buf+4));
      break;
    }
    else {
      printf("receive from server:#%d|%s|%ld#\n",*(int32_t*)buf, buf+12, *(int64_t*)(buf+4));
    }
  }
  gettimeofday(&tv_end, NULL);

  int64_t tc = tv_end.tv_sec - tv_begin.tv_sec + (tv_end.tv_usec - tv_end.tv_usec) / 1000000;
  printf("receive cost: %ld s.\n", tc);
  close(client_sockfd);
  return 0;
}


