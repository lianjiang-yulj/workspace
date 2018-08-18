#include "assert.h"
#include <sys/eventfd.h>
#include "bizClient.h"
#include <iostream>

using namespace std;

bizClient::bizClient(){
  m_finish = false;
  m_status = NULL;
  m_eventfd = eventfd(0,0);
}

bizClient::~bizClient(){
}

void bizClient::read(){
  MutexGuard guard(m_mutex);
  while(true){
    assert(*m_status == FREE);
    *m_status = READING;
    //while(*m_status != RESPOND && !m_finish){
    //  m_cond.wait(m_mutex);
    //} 
    uint64_t e = 1;
    ::read(m_eventfd,&e,sizeof(uint64_t));
    if(m_finish)
      return;
    //收到应答 可以发送下一条 
    *m_status = FREE; 
  }
}

void bizClient::respond(response * res){
  //写入文件
  *m_status = RESPOND;
  m_writer->put(res);
  uint64_t e = 1;
  write(m_eventfd,&e,sizeof(uint64_t));
  //m_cond.signal();
}

void bizClient::finish(){
  m_finish = true;
  //m_cond.signal(); 
  uint64_t e = 1;
  write(m_eventfd,&e,sizeof(uint64_t));
}


bool bizClient::isFinish(){
  return m_finish;
}
