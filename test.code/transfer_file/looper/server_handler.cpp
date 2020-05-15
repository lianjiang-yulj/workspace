#include "server_handler.h"
#include "protocal.h"
#include <iostream>
#include <string>

using namespace std;

Server::Server():v_start(512),v_length(512),v_line(512),v_pos(512),v_client(512){
}
Server::~Server(){}

void Server::write(looper * Looper,epoll_event * ev){
  Buffer *buf = this->m_writebuf;
  if(buf->readableSize()==0)
    return;
  int ret = buf->writeFd(m_fd);
  if(buf->readableSize()==0){
    buf->reset();
    Looper->deleteWrite(ev,m_fd);
  }
  return;
}

void Server::read(looper * Looper,epoll_event * ev){
  m_readbuf->shrink();
  int ret = m_readbuf->readFd(m_fd);
  if(ret==0){
    while(m_writebuf->readableSize()!=0)
      m_writebuf->writeFd(m_fd);
    Looper->Close(m_fd);
    return;

  }
  request req;


  m_writebuf->shrink();
  int clients = 0;
  while(decodeRequest(m_readbuf,req)){
    v_client[clients]=req.client_id;
    clients++;
  }

  ret = m_reader->get(v_start, v_length, v_line,v_pos,clients); 
  if(ret==0){
    //文件读完了
    while(m_writebuf->readableSize()!=0)
      m_writebuf->writeFd(m_fd);
    Looper->Close(m_fd);
    return;
  }

  for(int i=0;i<ret;i++){
    response res;
    res.line = v_line[i];
    res.client_id = v_client[i];
    res.pos = v_pos[i];

    // 截断
    int len = v_length[i]/3;
    res.length_first = len; 
    res.length_third = v_length[i]-2*len;
    res.first = m_reader->seek(v_start[i]);
    res.third = m_reader->seek(v_start[i]+2*len);
    encodeResponse(m_writebuf, res);
  }
  if(ret<clients-1){
    while(m_writebuf->readableSize()!=0)
      m_writebuf->writeFd(m_fd);
    Looper->Close(m_fd);
    return;
  }

  m_writebuf->writeFd(m_fd); 
  if(m_writebuf->readableSize()!=0)
    Looper->addWrite(ev,m_fd);
  else
    m_writebuf->reset();
}


