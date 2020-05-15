#ifndef __CONDITON_H_
#define __CONDITON_H_

#include <pthread.h>
#include <time.h>

namespace util{

namespace thread{

/**
* ???߳?ͬ??????????��?ļ򵥷?װ???ṩwait, timedWait, broadcast, signal?ӿ?
*/
class Condition
{
public:
    Condition()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
        pthread_mutex_init(&m_lock, &attr);    
        pthread_cond_init(&m_cond, NULL);
    }
    
    ~Condition()
    {
        pthread_mutex_destroy(&m_lock);
        pthread_cond_destroy(&m_cond);
    }
    
    /**
    * ?????ȴ???????��???趨
    * @return true??????��???趨?ɹ?
    */
    bool wait()
    {
        //pthread_mutex_lock(&m_lock);
        return (pthread_cond_wait(&m_cond, &m_lock) == 0);
    }
    /**
    * ?ȴ?tsʱ??????????��???趨
    * @param ts ?ȴ???ʱ??
    * @return true ?????ȴ??ɹ?
    */
    bool timedWait(const struct timespec *ts)
    {
        //pthread_mutex_lock(&m_lock);
        return (pthread_cond_timedwait(&m_cond, &m_lock, ts) == 0);
    }
    
    void endWait()
    {
        //pthread_mutex_unlock(&m_lock);
    }

    /**
    * ?㲥֪ͨ?????ڵȴ?????????��???߳?
    * @return true ?ɹ?
    */
    bool broadcast()
    {
        return (pthread_cond_broadcast(&m_cond) == 0);
    }
    /**
    * ֪ͨһ???ڵȴ?????????��???߳?
    */
    void signal()
    {
        pthread_cond_signal(&m_cond);
    }
    /**
    * ʹ????????��??????
    */
    void lock()
    {
        pthread_mutex_lock(&m_lock);
    }
    
    void unlock()
    {
        pthread_mutex_unlock(&m_lock);
    }
    
protected:
    pthread_mutex_t m_lock;
    pthread_cond_t  m_cond;
};
    
    
} //namespace thread
} //namespace util


#endif
