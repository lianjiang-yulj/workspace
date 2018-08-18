#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

class Mutex {
  public:
    Mutex()
    {
      pthread_mutex_init(&m_mutex,NULL);
    }
  
    ~Mutex(){
      pthread_mutex_destroy(&m_mutex);
    }

    void lock(){
      pthread_mutex_lock(&m_mutex);
    }
    
    void unlock(){
      pthread_mutex_unlock(&m_mutex);
    }
    
    pthread_mutex_t * get(){
      return &m_mutex;
    }
  private:
    pthread_mutex_t  m_mutex;
};

class MutexGuard{
  public:
    MutexGuard(Mutex & mutex):m_mutex(mutex){
      m_mutex.lock();
    }
    ~MutexGuard(){
      m_mutex.unlock();
    }
  private:
    Mutex &m_mutex;
};

class Condition{
  public:
    Condition(){
      pthread_cond_init(&m_cond,NULL);
    }

    ~Condition(){
      pthread_cond_destroy(&m_cond);
    }

    void wait(Mutex & mutex){
      pthread_cond_wait(&m_cond, mutex.get());
    }  

    void signal(){
      pthread_cond_signal(&m_cond);
    }
  private:
    pthread_cond_t m_cond;
};

#endif

