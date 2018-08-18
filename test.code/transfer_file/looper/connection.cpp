#include <iostream>
#include "connection.h"
#include "looper.h"
#include "protocal.h"

using namespace std;



void * client_func(void *arg){
  bizClient *client = (bizClient *)arg;
  //while(!client->isFinish()){
  client->read();
  //}
  return NULL;
}

Connection::Connection(){
  m_offset = 0;
  m_length = 0;
}
Connection::~Connection(){
}

int line_num(int n){
  if(n<10)
    return 1;
  else if(n<100)
    return 2;
  else if(n<1000)
    return 3;
  else if(n<10000)
    return 4;
  else if(n<100000)
    return 5;
  else if(n<1000000)
    return 6;
  else if(n<10000000)
    return 7;
  else if(n<100000000)
    return 8;
}

void Connection::read(looper * Looper,epoll_event *ev){
  Buffer * buf = this->m_readbuf;
  buf->shrink();
  int ret = buf->readFd(m_fd);
  if(ret == 0){
    m_writer->setMax(m_offset,m_length);
    for(int i=0;i<m_client.size();i++){
      m_client[i]->finish();
    }
    for(int i=0;i<m_client.size();i++){
      pthread_join(m_pid[i],NULL);
    }
    Looper->Close(m_fd);
    return;
  }
  response res;
  while(decodeResponse(buf, res)){
    assert(m_status[res.client_id] == WAITING);
    m_client[res.client_id]->respond(&res);
    //m_writer->put(&res);
    if(res.pos > m_offset){
      m_offset = res.pos;
      m_length = res.slice.getLen() + 2 +line_num(res.line);
      m_line = res.line;
      m_last = res.s;
    }
  }

  // 发送请求
  /*
  m_writebuf->shrink();
  for(int i=0;i<m_client.size(); i++){
    if(m_client[i]->ready() ){
      request req;
      req.client_id = i;
      cout<<"send client "<<i<<endl;
      encodeRequest(m_writebuf,req);
    }
  }
  m_writebuf->writeFd(m_fd);
  if(m_writebuf->readableSize()>0){
    Looper->addWrite(ev, m_fd);
  }
  */
}

void Connection::write(looper * Looper,epoll_event *ev){
  Buffer *buf = this->m_writebuf;

  request req;
  for(int i =0;i<m_client.size();i++){
    if(m_status[i] == READING){
      m_status[i] = WAITING;
      req.client_id = i;
      encodeRequest(buf,req);
    }
  }
  buf->writeFd(m_fd);
  if(buf->readableSize()==0){
    buf->reset();
    //Looper->deleteWrite(ev,m_fd);
  }
}

void Connection::setClientNum(int num, int cpu){
  m_client.resize(num);
  m_status.resize(num);
  for(int i=0;i<num;i++){
    bizClient *client = new bizClient();
    m_client[i] = client;
    m_status[i] = FREE;

    client->setWriter(m_writer);
    client->setStatus(&m_status[i]);
  }
  // 启动bizclient线程 用于模拟业务层的调用
  cpu_set_t cpu_info;
  CPU_ZERO(&cpu_info);
  CPU_SET(cpu, &cpu_info); 
  for(int i=0;i<m_client.size();i++){
    m_pid.push_back(pthread_t());
    pthread_create(&m_pid[i],NULL, client_func, m_client[i]);
    pthread_setaffinity_np(m_pid[i], sizeof(cpu_set_t), &cpu_info);
  }


}

// 用于发送第一次请求
void Connection::send(){

  return;
  for(int i=0;i<m_client.size(); i++){
    if(m_status[i]==READING){
      request req;
      req.client_id = i;
      encodeRequest(m_writebuf,req);
    }
  }


  while(m_writebuf->readableSize()>0){
    m_writebuf->writeFd(m_fd);
  }
  m_writebuf->reset();

}


