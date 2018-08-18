#include "Thread.h"

namespace util{

namespace thread{

Thread::Thread()
{
    m_callBackFn = NULL;
    m_callBackParam = NULL;
    m_control = NULL;
    m_end  = false;
    m_state = THREAD_TERM;
    m_tid = 0;
}

Thread::Thread(ThreadCallBack fn, void *param)
{
    m_end  = false;
    m_callBackFn = fn;
    m_callBackParam = param;
    m_control = NULL;
    m_state = THREAD_TERM;
    m_tid = 0;
}

Thread::~Thread()
{
    if (m_state != THREAD_TERM && m_tid != 0)
    {
        terminate();
        join();
    }
}

//for single thread job, don't need thread pool
pthread_t Thread::start(int nStackSize)
{
    if (m_state != THREAD_TERM || m_tid != 0)
    {
        return 0;
    }
        
    m_end = false;
    m_state = THREAD_INIT;
    pthread_attr_t   thread_attr;
    pthread_attr_init(&thread_attr); 
    pthread_attr_setstacksize(&thread_attr, nStackSize);
    
    int n = pthread_create(&m_tid, &thread_attr, thread_func, (void*)this);
    
    return (n == 0) ? m_tid : 0;
}

//for single thread job, don't need thread pool
void * Thread::thread_func(void *param)
{
    Thread *pthis = (Thread*)param;
    pthis->setState(THREAD_RUNNING);
    pthis->run();
    pthis->setState(THREAD_JOIN); //about to end
    return NULL;
}

pthread_t Thread::create(ThreadControl *pcontrol, int nStackSize)
{
    if (m_state != THREAD_TERM || m_tid != 0)
    {
        return 0;
    }
    
    m_end = false;
    m_state = THREAD_INIT;
    m_control = pcontrol;
    pthread_attr_t   thread_attr;
    pthread_attr_init(&thread_attr); 
    pthread_attr_setstacksize(&thread_attr, nStackSize);
        
    int n = pthread_create(&m_tid, &thread_attr, thread_pool_func, (void*)this);
    return (n == 0) ? m_tid : 0;
}

void * Thread::thread_pool_func(void *param)
{
    Thread *pthis = (Thread*)param;
    pthis->doRun();
    return NULL;
}

void Thread::doRun()
{
    while (!m_end)
    {
        //等待调度
        m_cond.lock();
        m_state = THREAD_IDLE;
        m_cond.wait();

        if (m_end) //可能是join或者cancel
        {
            m_cond.unlock();
            break;
        }
        //ok, 进入工作
        m_state = THREAD_RUNNING;
        run(); //用户可自定义函数指针，或者覆盖run方法
        m_state = THREAD_IDLE;
        m_control->complete(this); //通知线程池回收此线程
        m_cond.unlock();
    }
}

void Thread::activate(ThreadCallBack fn, void *param)
{
    if (fn)
    {
        m_callBackFn = fn;
        m_callBackParam = param; 
    }
    
    //THREAD_INIT状态表示线程正在创建，需要等待一下
    while (m_state == THREAD_INIT)
    {
        usleep(1);
    }
    //wake up the thread
    m_cond.signal();
}

void Thread::terminate()
{
    //THREAD_INIT状态表示线程正在创建，需要等待一下
    while (m_state == THREAD_INIT)
    {
        usleep(1);
    }
    
    //修改线程状态,需要加锁
    m_cond.lock();
    if (m_state == THREAD_TERM)
    {
        m_cond.unlock();
        return;
    }
    m_end = true;
    m_state = THREAD_TERM; //线程结束
    m_cond.unlock();
    m_cond.signal();
}

void Thread::join()
{
    pthread_join(m_tid, NULL); 
    m_tid = 0;
}

void Thread::cancel()
{
    if (m_state == THREAD_TERM)
    {
        return;
    }
    
    pthread_cancel(m_tid);
    m_state = THREAD_TERM;
    m_end = true;
    m_tid = 0;
}


} //namespace thread
} //namespace util

