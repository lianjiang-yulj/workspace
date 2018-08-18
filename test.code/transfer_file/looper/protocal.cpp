#include "protocal.h"
#include <iostream>

using namespace std;

bool decodeResponse(Buffer *buf, response & res){
  if(buf->readableSize()>=1){
    u_int8_t length = buf->peekUInt8();
    if(buf->readableSize()>=length){
      length = buf->readUInt8();
      res.line = buf->readUInt32();
      res.client_id = buf->readUInt8();
      res.pos = buf->readUInt32();
      //res.s = buf->readString();
      u_int8_t len = buf->readUInt8();
      res.slice.set(buf->readBegin(),len);
      buf->moveForward(len);
      return true;
    }
    else
      return false;
  }
  else
    return false;
}


bool decodeRequest(Buffer *buf, request & req){
  if(buf->readableSize()>=1){
    u_int8_t client_id = buf->readUInt8();
    req.client_id = client_id;
    return true;
  } 
  else
    return false;
}

void encodeRequest(Buffer *buf, const request & req){
  buf->appendUInt8(req.client_id);
  return;
} 

void encodeResponse(Buffer *buf, const response & res){
  u_int8_t length_line = res.length_first + res.length_third;
  u_int8_t length = 1 + 4 + 1 + 4 + 1 + length_line; // 总长度 + 行号 + 客户端id + 目标文件位置 + string
  buf->appendUInt8(length);
  buf->appendUInt32(res.line);
  buf->appendUInt8(res.client_id);
  buf->appendUInt32(res.pos);
  buf->appendUInt8(length_line);
  buf->append(res.first,res.length_first);
  buf->append(res.third,res.length_third);
}



