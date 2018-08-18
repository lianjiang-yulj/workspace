#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <iostream>
#include "fileReader.h"


using namespace std;
fileReader::fileReader():m_pos(40000000){
  m_buf = NULL;
  m_length = 0;
  m_TargetPos = 0;
  m_LenLinenumber = 1;
  counter = 0;
  total = 0;
}

fileReader::~fileReader(){
  if(m_buf != NULL){
    munmap(m_buf, m_length);
  }
}

bool fileReader::init(const string &filename){
  int fd = ::open(filename.c_str(),O_RDONLY);
  if(fd==-1){
    cerr<<"open file "<< filename <<" error "<< strerror(errno)<<endl;
    return -1;
  }
  struct stat buf;
  int ret = stat(filename.c_str(),&buf);
  if(ret==-1){
    cerr<<"stat file " << filename <<" error "<< strerror(errno)<<endl;
    close(fd);
    return -1;
  }
  m_length = buf.st_size;

  m_buf = (char *)mmap(0,buf.st_size,PROT_READ,MAP_SHARED,fd,0);
  if(m_buf ==  MAP_FAILED){
    cerr <<"mmap file "<<filename <<" error "<<strerror(errno)<<endl;
    close(fd);
    return -1;
  }
  close(fd);

  u_int32_t start = 0; 
  int length = 0;
  for(u_int32_t i=0; i<m_length; ){
    if(m_buf[i] == '\r'){
      length = i-start;
      m_pos[total] = make_pair(start,length);   
      start = i+2;
      i+=2;
      total++;
    }
    else
      i++;
  }
  return 0;

}

int fileReader::get(vector<u_int32_t> &start, vector<int> &len, vector<int> &line, vector<u_int32_t> &pos, int client){
  MutexGuard guard(m_mutex); 
  for(int i=0;i<client;i++){
    int index = counter++;
    line[i] = index;
    if(index>=total)
      return i;

    //if(unlikely(index % 100000 == 0))
    //  cout<<index<<endl;

    start[i] = m_pos[index].first;
    len[i] = m_pos[index].second;
    pos[i] = m_TargetPos; 
    m_TargetPos += m_LenLinenumber + (len[i]-len[i]/3)+2; 

    switch(counter){
      case 10:
      case 100:
      case 1000:
      case 10000:
      case 100000:
      case 1000000:
      case 10000000:
      case 100000000:
        {
          m_LenLinenumber++;
        }
    }

  }
  return client;
}

bool fileReader::get(int &start, int &len,int &line,int &pos){
  //int index = __sync_fetch_and_add(&counter,1); 
  int index = counter++;
  line = index;
  if(index>=total)
    return false;

  if(index % 100000 == 0)
    cout<<index<<endl;

  start = m_pos[index].first;
  len = m_pos[index].second;
  pos = m_TargetPos; 
  m_TargetPos += m_LenLinenumber + (len-len/3)+2; 

  switch(counter){
    case 10:
    case 100:
    case 1000:
    case 10000:
    case 100000:
    case 1000000:
    case 10000000:
    case 100000000:
      {
        m_LenLinenumber++;
      }
  }


  return true;
}

