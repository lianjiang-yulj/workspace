#ifndef PROTOCAL_H
#define PROTOCAL_H

#include <string>
#include "Buffer.h"

class Slice{
  public:
    Slice(){
      m_len = 0;
      m_buf = NULL;
    }
    Slice(char *c, u_int8_t len){
      c = m_buf;
      len = m_len;
    }
    ~Slice(){
    }

    void set(char * buf, u_int8_t len){
      m_buf = buf;
      m_len = len;
    }

    int getLen(){
      return m_len;
    }

    char * getBuf(){
      return m_buf;
    }
  private:
    u_int8_t m_len;
    char * m_buf;
};

// 服务端应答
struct response {
  u_int32_t line; // 行号
  u_int8_t client_id; // 该请求的客户端id
  // 数据内容
  int length_first;
  int length_third;
  char * first;
  char * third;
  std::string s;
  u_int32_t pos; //该行数据 在客户端文件中的位置
  Slice slice;
};

// 客户端请求
struct request {
  int client_id;
};

bool decodeResponse(Buffer *buf, response & res);


bool decodeRequest(Buffer *buf, request & req);

void encodeRequest(Buffer *buf, const request & req);

void encodeResponse(Buffer *buf, const response & res);

#endif 

