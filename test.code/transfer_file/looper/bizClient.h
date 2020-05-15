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
// ģ��ҵ��ʹ�õĿͻ��� ͨ������read������ȡ����˵���Ϣ
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
    void read(); //ҵ������

    bool isFinish();
    // ���¼���������ͨ�Ų����
    void respond(response *res); // �ÿͻ��˵�Ӧ���Ѿ��յ�
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
