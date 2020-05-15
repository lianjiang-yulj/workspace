#include "Buffer.h"
using namespace std;

Buffer::Buffer():m_vec(defaultLength){
  m_readable = 0;
  m_writable = 0;
}

Buffer::~Buffer() {
}


int Buffer::readFd(int fd){
  int ret = read(fd,writeBegin(), writableSize());
  if(ret>0)
    m_writable += ret;

  return ret;
}

int Buffer::writeFd(int fd){
  int ret = write(fd,readBegin(),readableSize());
  if(ret>0)
    m_readable += ret;   

  return ret;
}
void Buffer::reset(){
  m_readable = m_writable = 0;
}

u_int8_t Buffer::peekUInt8(){
  assert(readableSize() >= sizeof(u_int8_t));
  u_int8_t result = 0;
  memcpy(&result,readBegin(),sizeof(u_int8_t));
  return result;
}



int32_t Buffer::peekInt32(){
  assert(readableSize() >= sizeof(int32_t));
  int32_t result = 0;
  memcpy(&result,readBegin(),sizeof(int32_t));
  return result;
}

u_int32_t Buffer::peekUInt32(){
  assert(readableSize() >= sizeof(u_int32_t));
  u_int32_t result = 0;
  memcpy(&result,readBegin(),sizeof(u_int32_t));
  return result;
}

u_int8_t Buffer::readUInt8() {
  u_int8_t result = peekUInt8();
  m_readable += sizeof(u_int8_t); 
  return result;
}


int32_t Buffer::readInt32() {
  int32_t result = peekInt32();
  m_readable += sizeof(int32_t); 
  return result;
}

u_int32_t Buffer::readUInt32() {
  u_int32_t result = peekUInt32();
  m_readable += sizeof(u_int32_t); 
  return result;
}

std::string Buffer::readString() {
  int32_t len = readInt32(); 
  assert(readableSize()>=len);
  std::string s(readBegin(),len);
  m_readable += len;
  return s;
}

void Buffer::appendString(const std::string &s){
  appendInt32(s.length());
  append(s.c_str(),s.length());
}

void Buffer::append(const char * c, int n){
  assert(writableSize()>=n);
  std::copy(c, c+n,writeBegin());
  m_writable += n;

}

void Buffer::appendUInt32(u_int32_t n){
  append((char *)&n,sizeof(u_int32_t));
  return;
}

void Buffer::appendInt32(int32_t n){
  append((char *)&n,sizeof(int32_t));
  return;
}

void Buffer::appendUInt8(u_int8_t n){
  append((char *)&n,sizeof(u_int8_t));
  return;
}

char * Buffer::readBegin(){
  return &*m_vec.begin() + m_readable;
}

char * Buffer::writeBegin(){
  return &*m_vec.begin() + m_writable;
}

int Buffer::readableSize() {
  return m_writable - m_readable;
}

int Buffer::writableSize() {
  return m_vec.size() - m_writable;
}


void Buffer::forceShrink(){
  int readSize = readableSize();
  std::copy(m_vec.begin()+ m_readable ,m_vec.begin()+ m_writable, m_vec.begin());
  m_readable = 0;
  m_writable = readSize;
  return;
}
void Buffer::shrink(){
  if(readHoleSize()>= 60000){
    forceShrink();
  }
}

int Buffer::readHoleSize(){
  return m_readable;
}


