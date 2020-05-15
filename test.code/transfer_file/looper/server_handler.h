#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include "looper.h"
#include "handler.h"
#include "fileReader.h"

class Server : public Handler {
  public:
    Server();
    virtual ~Server();
    virtual void read(looper * looper,epoll_event *ev); 
    virtual void write(looper * looper,epoll_event *ev); 

    void setReader(fileReader * reader){
      m_reader = reader;
    }
  private:
    fileReader *m_reader;
    std::vector<u_int32_t> v_start;
    std::vector<int> v_length;
    std::vector<int> v_line;
    std::vector<u_int32_t> v_pos;
    std::vector<int> v_client;

};

#endif
