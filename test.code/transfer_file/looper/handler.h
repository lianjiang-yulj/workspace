#ifndef HANDLER_H
#define HANDLER_H

#include "Buffer.h"
#include <sys/epoll.h>

class looper;

class Handler {
  public:
    Handler(){}
    virtual ~Handler() {};

    virtual void read(looper * looper,epoll_event *ev) = 0;
    virtual void write(looper * looper,epoll_event *ev) = 0;
    
    void setReadBuf(Buffer * buf){
      m_readbuf = buf;
    }

    void setWriteBuf(Buffer *buf){
      m_writebuf = buf;
    }

    void setFD(int fd){
      m_fd = fd;
    }

    int getFD(){
      return m_fd;
    }

  protected:
    Buffer * m_readbuf;
    Buffer * m_writebuf;
    int m_fd;
};

#endif

