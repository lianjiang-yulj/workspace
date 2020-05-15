#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/tcp.h>

#include <iostream>
#include <fstream>
#include <string>
#include "common.h"
#include "looper.h"
#include "Buffer.h"
#include "connection.h"
#include "fileWriter.h"

using namespace std;


string server = "localhost";
int port = 5555;
string output = "result";


void *runner(void *arg){
  looper * l= (looper *)arg;
  l->loop();
  return NULL;
}


int main(int argc, char **argv){
  if(argc != 4){
    cerr<<"usage ip port output"<<endl;
    exit(1);
  }

  //printTime();
  timeval start,end;
  gettimeofday(&start,NULL);


  server = argv[1];
  port = atoi(argv[2]);
  output = argv[3];

  signal(SIGPIPE, SIG_IGN);
  int work_num = 12; // 工作线程数
  int core_num = 24; // cpu的数量
  int cpu_index = 0;
  cpu_set_t cpu_info;

  looper looper[work_num];
  pthread_t looper_pid[work_num];

  for(int i=0;i<work_num;i++){
    pthread_create(&looper_pid[i],NULL,runner,&looper[i]); 
    CPU_ZERO(&cpu_info);
    CPU_SET(cpu_index%core_num, &cpu_info);
    cpu_index++;
    pthread_setaffinity_np(looper_pid[i], sizeof(cpu_set_t), &cpu_info);
  }

  fileWriter writer(output);
  int client_num = 12; //连接数量
  for(int i=0;i<client_num;i++){
    int sock = connectOrDie(server,port);
    setNonBlocking(sock);
    int optval = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
    
    Connection *conn = new Connection();

    conn->setFD(sock);
    conn->setReadBuf(new Buffer());
    conn->setWriteBuf(new Buffer());
    conn->setWriter(&writer);
    conn->setClientNum(200,cpu_index % core_num);
    cpu_index++;
    //conn->send();
    looper[i%work_num].addRead(conn);
  }

  for(int i=0;i<work_num;i++){
    pthread_join(looper_pid[i],NULL);
  }

  gettimeofday(&end,NULL);
  //cout<<"cost "<< (end.tv_sec-start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000<<endl;

  writer.close();

  //printTime();
  gettimeofday(&end,NULL);
  cout<<"cost="<< (end.tv_sec-start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000<<endl;

  return 0;
}
