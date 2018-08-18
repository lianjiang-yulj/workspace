#ifndef __THREAD_POOL_H_
#define __THREAD_POOL_H_

#include <list>
#include <assert.h>
#include "Lock.h"
#include "Condition.h"
#include "Thread.h"

namespace util{
namespace thread{

const int INIT_THREAD_NUM = 16;
const int MAX_THREAD_NUM = 1023;


/**
* ThreadPool: һ?δ????????̣߳??߳??????????󣬿??????±?????
*/
template<class T> 
class ThreadPool : public ThreadControl
{
protected:
    template<class THR>
    struct ListThread
    {
        THR * pthreads;
        int threadNum;
        ListThread<THR> *prev;
        ListThread(int num, ListThread<THR> *p)
        {
            threadNum = num;
            pthreads = new THR [threadNum];
            this->prev = p;
        }
        ~ListThread()
        {
            delete [] pthreads;
        }
    };    
public:   
    
    /**
     * ???캯??
     * @param initThreadNum ??ʼ?????̸߳?????֮??ÿ??????ʱҲ??????????ֵ?????µ??߳?
     */
    ThreadPool(int32_t initThreadNum = INIT_THREAD_NUM, int nMaxThreadNum = MAX_THREAD_NUM, int nStackSize = INIT_THREAD_SIZE)
    {
        m_nTotalThreadNum = 0;
        
        //??initThreadNum????????
        if (initThreadNum <= 0)
        {
            m_initThreadNum = 1;
        }
        else if (initThreadNum > MAX_THREAD_NUM)
        {
            m_initThreadNum = MAX_THREAD_NUM;
        }
        else
        {
            m_initThreadNum = initThreadNum;
        }

        //??nMaxThreadNum????????
        if(nMaxThreadNum < m_initThreadNum)
        {
            m_nMaxThreadNum = m_initThreadNum;
        }
        else if (nMaxThreadNum > MAX_THREAD_NUM)
        {
            m_nMaxThreadNum = MAX_THREAD_NUM;
        }
        else
        {
            m_nMaxThreadNum = nMaxThreadNum;
        }

        m_nStackSize = nStackSize;
        m_pListThreads = NULL;
        makeNewThreads(m_initThreadNum);
    }

    ~ThreadPool()
    {
        while(m_pListThreads != NULL)
        {
            ListThread<T> *prev = m_pListThreads->prev;
            delete m_pListThreads;
            m_pListThreads = prev;
        }        
    }
    
    /**
     * ??thread pool??ȡ??һ???????߳?.
     * @param activate ?Ƿ񼤻????̣߳????�����???????????Ҫ????Thread::activate()��???????߳?
     * @return ????Thread??ָ??
     * @return NULL ???????ܷ????µ??߳?
     */
    T *getThread(bool activate = false)
    {
        AutoLock aLock(&m_lock); //we have to lock this function
        if (m_idleThread.empty() && !makeNewThreads(m_initThreadNum))
        {
            //error happens
            return NULL;
        }
        T *p = m_idleThread.front();
        m_idleThread.pop_front();
        assert(p);
        if (activate)
            p->activate(); //activate this thread ?
        return p;
    }
    
    /**
     * ?߳????н???????Ҫ?����???????????��???Լ?????
     */
    void complete(Thread *pThread)
    {
        AutoLock aLock(&m_lock); //we have to lock this function
        m_idleThread.push_back((T*)pThread); 
    }

    /**
     * ??ȡ?ܹ????????߳???��
     *@return ???߳???
     */
    int getTotalThreadNum()
    {
        return m_nTotalThreadNum;
    }

    /**
     * ???ػ??ж???ʣ???Ŀ????߳̿???
     *@return ???߳???
     */
    int getIdleThreadNum()
    {
        return m_idleThread.size();
    }

protected:
    
    bool makeNewThreads(int num)
    {
        try
        {
            if (m_nTotalThreadNum + num > m_nMaxThreadNum)
            {
                if (m_nMaxThreadNum - m_nTotalThreadNum <= 0)
                {
                    return false;
                }
                num = m_nMaxThreadNum - m_nTotalThreadNum;
            }
            
            m_pListThreads = new ListThread<T> (num, m_pListThreads);
            T * pT = m_pListThreads->pthreads;
            for (int i = 0; i < num; i++)
            {
                (pT + i)->create(this, m_nStackSize);//ָ??thread control
                m_idleThread.push_back(pT + i);
                m_nTotalThreadNum++;
                usleep(1);//???߳???????��
            }
        }
        catch(...)
        {
            return false;
        }
        return true;
    }

    util::thread:: Condition      m_cond;    
    util::thread:: Lock           m_lock;
    std::list<T*>  m_idleThread;
    ListThread<T>* m_pListThreads;
    int32_t          m_initThreadNum;
    int32_t          m_nMaxThreadNum;
    int32_t          m_nTotalThreadNum;
    int32_t          m_nStackSize;
};

} //namespace thread
} //namespace ali

#endif

