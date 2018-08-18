#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "protocal.h"
#include "Mutex.h"
#include <string>

class fileWriter{
  public:
    /*
    fileWriter(std::string output):m_vec(20000000){
      ofile.open(output.c_str());
        
    }
    ~fileWriter(){
      ofile.close();
    }
    */

    fileWriter(const std::string &filename);

    ~fileWriter();

    //void put(int num, const std::string &s);
    void put(response *res);
    void setMax(u_int32_t pos,int length);
    void close();
  private:
    //std::vector<std::string> m_vec;
    //std::ofstream ofile;
    char * m_buf;
    int m_fd;
    long long m_size;
    Mutex m_mutex;
    u_int32_t m_final;
    std::string  m_end;
};


#endif
