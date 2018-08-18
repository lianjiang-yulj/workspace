#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <string.h>
#include <stdlib.h>
#include <stdio.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

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
  
    unsigned char tmp[1024];
    while(1)
	{
		printf("\nPlease input the message:");
		scanf("%s",buf);
 //       strcpy(buf, "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000gggggg");
        printf("req:#%s#\n", buf);
		send(client_sockfd,buf,BUFFER_SIZE,0);
	
	 	len=recv(client_sockfd,buf,BUFFER_SIZE,0);
        buf[len] = '\0';
        if (strncmp(buf, "NODATA", 6) == 0) {
            printf("receive from server:#%s#\n",buf);
        }
        else {
            strncpy(tmp, buf+4, len-12);
            tmp[len-12] = '\0';
            printf("receive from server:#%d|%s|%ld#\n",*(int32_t*)buf, tmp, *(int64_t*)(buf+len-8));
        }

		if(strcmp(buf,"exit")==0)
		{
			break;
		}
		if(len<=0)
		{
			perror("receive from server failed");
			exit(EXIT_FAILURE);
		}
	}
    close(client_sockfd);
    return 0;
}


