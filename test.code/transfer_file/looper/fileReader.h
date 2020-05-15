#ifndef FILEREADER_H
#define FILEREADER_H

#include <fstream>
#include <vector>
#include <string>
#include "pthread.h"
#include "Mutex.h"
#include <stdlib.h>

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

class fileReader {
  public:
    fileReader (); 

    ~fileReader();
       

    bool init(const std::string &filename);


    char * seek(long long pos){
      return m_buf + pos;
    }

    bool get(int &start, int &length, int &line,int &pos);
    int get(std::vector<u_int32_t> &start, std::vector<int> &length, std::vector<int> &line, std::vector<u_int32_t> &pos, int client);
     
  private:
    std::vector<std::pair<u_int32_t, int > > m_pos; 
    std::ifstream ifile;
    Mutex m_mutex;
    u_int32_t counter;
    u_int32_t total;
    char * m_buf;
    u_int32_t m_length;
    u_int32_t m_TargetPos;
    int m_LenLinenumber;
};



#endif
