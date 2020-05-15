#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "fileWriter.h"

using namespace std;

fileWriter::fileWriter(const string &filename):m_buf(NULL),m_fd(-1){
  m_size =(long long) 4*1024*1024*1024; //4G 
  m_fd = open(filename.c_str(), O_RDWR|O_TRUNC|O_CREAT, 0664);
  m_final = 0;
  if(m_fd == -1){
    cerr << "open file " << filename << " error "<< strerror(errno)<<endl;
  }
  m_end = "\r\n";

  ftruncate(m_fd, m_size);

  m_buf =(char *) mmap(0,m_size, PROT_WRITE, MAP_SHARED, m_fd,0);
  if(m_buf == MAP_FAILED){
    cerr<<"mmap file "<<filename <<" error "<<strerror(errno)<<endl;
  } 
  
  
}

fileWriter::~fileWriter(){
}

void fileWriter::close(){
  munmap(m_buf, m_size);
  ftruncate(m_fd, m_final);
  ::close(m_fd);
}

void fileWriter::put(response * res){
  //string &s = res->s;
  Slice slice = res->slice;
  //reverse(s.begin(), s.end());
  u_int32_t pos = res->pos;
  /*
  ostringstream oss;
  oss<<res->line;
  string line = oss.str();
  oss.clear();
  memcpy(m_buf+pos,line.c_str(), line.length());
  pos += line.length();
  */
  int ret = snprintf(m_buf+pos,12,"%d",res->line);
  pos += ret;
  //reverse_copy(s.begin(),s.end(),m_buf+pos);
  reverse_copy(slice.getBuf(),slice.getBuf()+slice.getLen(),m_buf+pos);
  //memcpy(m_buf+pos,s.c_str(), s.length());
  pos += slice.getLen();
  memcpy(m_buf+pos,m_end.c_str(),2);
 // m_vec[num] = s;
}

void fileWriter::setMax(u_int32_t pos, int length){
  MutexGuard guard(m_mutex);
  if(pos+length>m_final){
    m_final = pos+length;
  }
}

