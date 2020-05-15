#ifndef BUFFER_H
#define BUFFER_H

#include <string.h>
#include <assert.h>
#include <vector>
#include <string>
#include <algorithm>

class Buffer {
  public:
    static const int defaultLength = 125350; //128k
    Buffer();
    ~Buffer();

    int readFd(int fd);
    int writeFd(int fd);
    void reset();

    u_int8_t peekUInt8();    
    u_int8_t readUInt8();
    int32_t peekInt32();
    int32_t readInt32();

    u_int32_t peekUInt32();
    u_int32_t readUInt32();

    std::string readString(); 

    void appendString(const std::string &s);     

    void append(const char * c, int n);

    void appendInt32(int32_t n);
    void appendUInt32(u_int32_t n);

    void appendUInt8(u_int8_t n);

    char * readBegin();

    char * writeBegin();    

    int readableSize();    

    int writableSize();

    void forceShrink();

    void shrink();
    int readHoleSize();

    void moveForward(int n){
      m_readable += n;
    } // m_readable move
  private:
    std::vector<char> m_vec;
    int m_readable;
    int m_writable;
     
};
#endif

