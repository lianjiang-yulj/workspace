#include "Lock.h"

namespace util{

namespace thread{

//btry means if we just make a try
AutoLock::AutoLock(Lock *lock, bool btry) : plock(lock), ok(true)
{
    if (btry)
    {
        ok = plock->tryLock();
    }
    else{
        plock->lock();
    }
}

AutoLock::~AutoLock()
{
    if (ok)
        plock->unlock();
}
    
AutoReaderLock::AutoReaderLock(RWLock *lock, bool btry):plock(lock), ok(true)
{
    if (btry)
    {
        ok = plock->tryLockRead();
    }else{
        plock->lockRead();
    }
}

AutoReaderLock::~AutoReaderLock()
{
    if (ok)
        plock->unlock();
}

AutoWriterLock::AutoWriterLock(RWLock *lock, bool btry):plock(lock), ok(true)
{
    if (btry)
    {
        ok = plock->tryLockWrite();
    }else{
        plock->lockWrite();
    }
}

AutoWriterLock::~AutoWriterLock()
{
    if (ok)
        plock->unlock();
}

}//namespace thread

}//namespace util
