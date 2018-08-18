#ifndef __LOCK_H_
#define __LOCK_H_

#include <pthread.h>
#include <time.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>


namespace util{
    
namespace thread{

/**
* Interface class, ֧???߳̿?????????
*/
class Lock
{
public:
    /**
    * ?????????????߳???
    */
    Lock()
    {
        //const pthread_mutexattr_t attr = { PTHREAD_MUTEX_RECURSIVE_NP };
        //pthread_mutex_init(&mutex, &attr);
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
        pthread_mutex_init(&mutex, &attr);
        
    }
    ~Lock()
    {
        pthread_mutex_destroy(&mutex);
    }
    
    /**
    * ?????????Ѿ????�����???߳̽??ȴ?
    */
    void lock()
    {
        pthread_mutex_lock(&mutex);
    }
    /**
    * ??????????ָ????ʱ????δ??ȡ?????򷵻?false
    * @return true?ɹ???ȡ??
    */
#ifdef __USE_XOPEN2K    
    bool timedLock(const struct timespec* ts)
    {
        return (pthread_mutex_timedlock(&mutex, ts) == 0);
    }
#endif
    
    /**
    * ????û?л?ȡ????????��?̷???
    * @return true?????ɹ?????, false????ʧ??
    */
    bool tryLock()
    {
        return (pthread_mutex_trylock(&mutex) == 0);
    }
    
    void unlock()
    {
        pthread_mutex_unlock(&mutex);
    }

protected:
    pthread_mutex_t mutex;
};

/**
* Interface class, ֧?ֽ??̼??Ļ??⣬ע?⣬?ӿں?Lock?Ĳ?һ?????????ȵ???init???г?ʼ???????ж??Ƿ??ɹ?
*/
class ProcessLock
{
public:
    ProcessLock()
    {   
        m_pMutex = NULL;
        m_szShmAddr = NULL;
        m_nShmId = -1;
        m_nShmKey = 0;   
    }
    ~ProcessLock()
    {
        destroy();
    }

    /**
    * ??ʼ??????Ϊ?Ƿ??ڹ????ڴ??еģ???ʹ?õ?ʱ???????ж??Ƿ???ʼ???ɹ?
    * ????MD5ֵ
    * @param key_t key ָ???????ڴ???key
    * @return true:??ʼ???ɹ???false:??ʼ??ʧ??
    */
    bool init(key_t key)
    {
        bool bCreate = true;
        m_nShmKey = key;

        m_nShmId = shmget(m_nShmKey, sizeof(pthread_mutex_t), 0777 | IPC_CREAT | IPC_EXCL); 
        if ((m_nShmId == -1) && (errno == EEXIST))
        {
            m_nShmId = shmget(m_nShmKey, 0, 0);
            bCreate = false;
        }

        if (-1 == m_nShmId)
        {
            fprintf(stderr, "Error : shmget  error for processLock, key = %d \n", m_nShmKey);
            return false;
        }
        
        m_szShmAddr = (char *)shmat(m_nShmId, NULL, 0);
        if(m_szShmAddr == (char*)-1) 
        {
            fprintf(stderr, "Error : shmat error for processLock\n");
            return false;
        }
        m_pMutex = (pthread_mutex_t *)m_szShmAddr;

        struct shmid_ds ds;
        shmctl(m_nShmId, IPC_STAT, &ds);
        if (bCreate || (ds.shm_nattch == 1))
        {
            pthread_mutexattr_t attr;
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            //????????Ϊ???̼??ģ?PTHREAD_PROCESS_SHARED
            pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
            pthread_mutex_init(m_pMutex, &attr);
        }
        return true;
    }

    /**
    * destroy?????ڴ???????Դ
    */
    void destroy()
    {
        //???չ????ڴ?
        if (m_nShmId != -1 && m_szShmAddr != NULL) 
        {
            struct shmid_ds ds;
            shmctl(m_nShmId, IPC_STAT, &ds);
        
            if(ds.shm_nattch == 1)
            {
                pthread_mutex_destroy(m_pMutex);
        
                memset(m_szShmAddr, 0, sizeof(pthread_mutex_t));
                shmdt(m_szShmAddr);    
                shmctl(m_nShmId, IPC_RMID, NULL);
                
                m_pMutex = NULL;
                m_szShmAddr = NULL;
                m_nShmId = -1;
            }
        }
    }
    
    /**
    * ?????????Ѿ????�����???߳̽??ȴ?
    * @return true?????ɹ?????, false????ʧ?ܣ???ʾ???????쳣????????????ռ??????û???ͷŵ??쳣?˳??ˣ?
    */
    bool lock()
    {
        if (pthread_mutex_trylock(m_pMutex) == 0)
        {
            return true;
        }

        //???????ɹ????п????????Ľ????˳??ˣ?????û???ͷ???????
        struct shmid_ds ds;
        shmctl(m_nShmId, IPC_STAT, &ds);

        //˵??ֻ????????????????????,??????
        if(ds.shm_nattch == 1)
        {
            return false;
        }

        pthread_mutex_lock(m_pMutex);
        return true;
    }
    /**
    * ??????????ָ????ʱ????δ??ȡ?????򷵻?false
    * @return true?ɹ???ȡ??
    */
#ifdef __USE_XOPEN2K    
    bool timedLock(const struct timespec* ts)
    {
        return (pthread_mutex_timedlock(m_pMutex, ts) == 0);
    }
#endif
    
    /**
    * tryLock??????û?л?ȡ????????��?̷???
    * @return true?????ɹ?????, false????ʧ??
    */
    bool tryLock()
    {
        return (pthread_mutex_trylock(m_pMutex) == 0);
    }

    /**
    * ????
    */
    void unlock()
    {
        pthread_mutex_unlock(m_pMutex);
    }

protected:
    pthread_mutex_t* m_pMutex;
    int m_nShmId;
    char *m_szShmAddr;
    key_t m_nShmKey;
};

/**
* Read write lock,
*/
class RWLock
{
public:

    RWLock()
    {
        pthread_rwlock_init(&rwlock, NULL);
    }
    
    ~RWLock()
    {
        pthread_rwlock_destroy(&rwlock);
    }
    
    /**
    * lockRead:һ???߳????????ε?????lockRead??????????????ͬ?????ε?unlock()????
    * @see unlock()
    * @see lockWrite()
    */
    void lockRead()
    {
        pthread_rwlock_rdlock(&rwlock);
    }
    
    /**
    * lockWrite:һ???߳????????ε?????lockWrite??????????????ͬ?????ε?unlock()????
    * @see unlock()
    * @see lockRead()
    */
    void lockWrite()
    {
        pthread_rwlock_wrlock(&rwlock);
    }
    
    bool tryLockRead()
    {
        return (pthread_rwlock_tryrdlock(&rwlock) == 0);
    }
    
    bool tryLockWrite()
    {
        return (pthread_rwlock_trywrlock(&rwlock) == 0);
    }
    
#ifdef __USE_XOPEN2K    
    bool timedLockRead(const struct timespec* ts)
    {
        return (pthread_rwlock_timedrdlock(&rwlock, ts) == 0);
    }
    
    bool timeLockWrite(const struct timespec* ts)
    {
        return (pthread_rwlock_timedwrlock(&rwlock, ts) == 0);
    }
#endif
    
    void unlock()
    {
        pthread_rwlock_unlock(&rwlock);
    }
    
protected:
    pthread_rwlock_t rwlock;
};

/**
* AutoLock, ?ں???????ʹ??AutoLock��??֤????????????ʹ??{}��?޶?AutoLock??????????
*/
struct AutoLock
{
    AutoLock(Lock *lock, bool btry = false);
    ~AutoLock();
    
protected:
    Lock *plock;
    bool ok;
};

/**
* AutoReaderLock, ͬAutoLock??Ϊ???̼߳ӵ??Զ?????
*/
struct AutoReaderLock
{
    AutoReaderLock(RWLock *lock, bool btry = false);
    ~AutoReaderLock();
    
protected:
    RWLock *plock;
    bool ok;
};

/**
* AutoWriterLock, ͬAutoLock??Ϊд?̼߳ӵ??Զ?д??
*/
struct AutoWriterLock
{
    AutoWriterLock(RWLock *lock, bool btry = false);
    ~AutoWriterLock();
    
protected:
    RWLock *plock;
    bool ok;
};

} //namespace thread

} //namespace util


#endif
