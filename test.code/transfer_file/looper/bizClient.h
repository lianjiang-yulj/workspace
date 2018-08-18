#ifndef BIZ_CLIENT_H
#define BIZ_CLIENT_H


#include "protocal.h"
#include "Mutex.h"
#include "fileWriter.h"

enum STATUS{
  FREE = 0, 
  READING = 1,
  WAITING = 2,
  RESPOND = 3,
};
// 模拟业务使用的客户端 通过调用read函数获取服务端的消息
class bizClient {
  public:
    bizClient();
    ~bizClient();

    void setStatus(int *status){
      m_status = status;
    }
    int getStatus(){
      return *m_status;
    }

    void setWriter(fileWriter *writer){
      m_writer = writer;
    }
    void read(); //业务层调用

    bool isFinish();
    // 以下几个函数由通信层调用
    void respond(response *res); // 该客户端的应答已经收到
    void finish();
    
  private:
    Mutex m_mutex;
    Condition m_cond;
    int *m_status;
    bool m_finish;
    fileWriter *m_writer;
    int m_eventfd;
};
#endif
