#ifndef LOOPER_H
#define LOOPER_H

#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <queue>
#include "handler.h"

class looper {
  public:
    looper(){
      m_counter=0;
      stop = false;
      m_fd = epoll_create(512);;
      if(m_fd == -1){
        std::cerr << "create epoll error " << strerror(errno) << std::endl;
      }
      pthread_mutex_init(&m_mutex,NULL);
    }

    ~looper(){
      pthread_mutex_destroy(&m_mutex);
    } 

    void Close(int fd){
      epoll_ctl(m_fd, EPOLL_CTL_DEL,fd,NULL);
      close(fd);
      m_counter--;
      if(m_counter==0)
        stop = true;
    }

    void addRead(Handler *handler){
      pthread_mutex_lock(&m_mutex);
      m_q.push(handler);
      pthread_mutex_unlock(&m_mutex);
    }

    void addWrite(epoll_event * ev,int fd){
      ev->events |= EPOLLOUT;  
      epoll_ctl(m_fd, EPOLL_CTL_MOD,fd,ev);
      return;
    }

    void deleteWrite(epoll_event * ev,int fd){
      ev->events &=  ~EPOLLOUT;  
      epoll_ctl(m_fd, EPOLL_CTL_MOD,fd,ev);
      return;
    }

    void loop(){
    
      int event_num = 32;
      epoll_event ev[event_num];
      while(!stop){
        //if(m_start==true && m_counter==0)
        //  break;
          
        // do io 
        int num = epoll_wait(m_fd, ev, event_num,100);
        for(int i=0;i<num;i++){
          Handler * hander = (Handler *)ev[i].data.ptr;
          if(ev[i].events & EPOLLIN){
            hander->read(this,&ev[i]); 
          }
          if(ev[i].events & EPOLLOUT){
            hander->write(this,&ev[i]); 
          }
        }
        
        // add new fd
        pthread_mutex_lock(&m_mutex);
        while(!m_q.empty()){
          Handler * handler = m_q.front();
          m_q.pop();
          epoll_event event;
          event.data.ptr = handler;
          event.events = EPOLLIN | EPOLLOUT;
          // uncheck 
          int ret= epoll_ctl(m_fd, EPOLL_CTL_ADD,handler->getFD(),&event);
          if(ret== -1){
            std::cout<<"epoll ctl add error" << strerror(errno)<< std::endl;
          }
          m_counter++;
        }
        pthread_mutex_unlock(&m_mutex);
      } // end while true
      return;
    }
  private:
    int m_fd;
    std::queue<Handler*> m_q;
    pthread_mutex_t m_mutex;
    bool stop;
    int m_counter;
    
};

#endif
