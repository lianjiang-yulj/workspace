#ifndef __THREAD_H_
#define __THREAD_H_

#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "Condition.h"

const int INIT_THREAD_SIZE = 1024*1024*10;


namespace util{
    
namespace thread{

enum ThreadState
{
    THREAD_INIT,
    THREAD_IDLE,
    THREAD_RUNNING,
    THREAD_JOIN,
    THREAD_TERM,
    THREAD_CANCELED
};

class Thread;

typedef void (*ThreadCallBack)(void *);

/**
* ThreadControlʵ??????ThreadPool?Ľӿ?
*/
class ThreadControl
{
public:
    ThreadControl(){}
    virtual ~ThreadControl(){}
    virtual void complete(Thread *p){}
};

    
class Thread
{
public:
    Thread();
    Thread(ThreadCallBack fn, void *param);
    virtual ~Thread();

    /**
     * ֪ͨ?߳̽???
     */
    virtual void terminate();


    /**
     * ?????Ѿ??????̵߳???Դ
     */
    virtual void join();

    /**
     *  ?????̣߳?ֱ??????
     */
    virtual pthread_t start(int nStackSize = INIT_THREAD_SIZE);

    /**
     *  ?????̣߳???ThreadPool???????ȴ???????��?????ѽ??е???
     */
    virtual pthread_t create(ThreadControl *pcontrol, int nStackSize = INIT_THREAD_SIZE);

    /**
     *  ʵ????һ??commandģʽ???û??̳?Thread??????run??????д???????߼?
     */
    virtual int run() 
    {
        if (m_callBackFn)
        {
            m_callBackFn(m_callBackParam);
        }
        return 0;
    }

    /**
     *  ?????̣߳??û??̳߳صĵ???
     */
    void activate(ThreadCallBack fn = NULL, void *param = NULL);

    /**
     *  ??ֹ?̣߳??û??̳߳صĵ???
     */
    void cancel();

    /**
     *  ??ȡ?̵߳?????״̬, ?????̵߳?״̬
     */
    const ThreadState getState() const{ return m_state; }
    void  setState(ThreadState state) { m_state = state; }
    const int getThreadID() const {return m_tid;}

protected:
    /**
     *  Ϊ?̳߳?ʹ?õĵ??Ⱥ???
     */
    void doRun();
    
    /**
     *  ??ͨ?߳???ʹ?õ????ں???
     */
    static void * thread_func(void *param);
    
    /**
     *  ?̳߳???ʹ?õ????ں???
     */
    static void * thread_pool_func(void *param);

protected:
    pthread_t       m_tid;
    ThreadState     m_state;
    Condition       m_cond;
    ThreadControl  *m_control;
    ThreadCallBack  m_callBackFn;
    void           *m_callBackParam;
    bool            m_end;
};
    
} //namespace thread
} //namespace util

#endif
