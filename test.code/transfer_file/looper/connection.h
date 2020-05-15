#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/epoll.h>
#include "handler.h"
#include "bizClient.h"
#include "fileWriter.h"

class Connection :public Handler {
  public:
    Connection();
    virtual ~Connection();

    void setClientNum(int num,int cpu);
    void send();

    virtual void read(looper * looper,epoll_event *ev);
    virtual void write(looper * looper,epoll_event *ev);
    void setWriter(fileWriter *writer){
      m_writer = writer;
    }
  private:
    //std::vector<int> m_client;
    std::vector<bizClient *> m_client;
    std::vector<int> m_status;
    std::vector<pthread_t> m_pid;
    fileWriter * m_writer;
    u_int32_t m_offset;
    u_int32_t m_length;
    int m_line;
    std::string m_last;
};
#endif
