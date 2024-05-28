#ifndef __COMMON_READWRITE_LOCKER_H__
#define __COMMON_READWRITE_LOCKER_H__

#include <common_global.h>

COMMON_BEGIN_NAMESPACE

class COMMON_EXPORT ReadWriteLock
{
    DISABLE_COPY(ReadWriteLock)
public:
    ReadWriteLock();
    virtual ~ReadWriteLock();

    void lockForRead();
    void lockForWrite();
    void unlock();

    bool tryLockForRead();
    bool tryLockForWrite();
private:
    class ReadWriteLockImpl* d;
};

class COMMON_EXPORT ReadLocker
{
    DISABLE_COPY(ReadLocker)
public:
    ReadLocker(ReadWriteLock& lock)
        : lock_(&lock)
    {
        lock_->lockForRead();
    }
    virtual ~ReadLocker(){lock_->unlock();}
private:
    ReadWriteLock* lock_;
};

class COMMON_EXPORT WriteLocker
{
    DISABLE_COPY(WriteLocker)
public:
    WriteLocker(ReadWriteLock& lock)
        : lock_(&lock)
    {
        lock_->lockForWrite();
    }
    void unLocker(){lock_->unlock();}
    virtual ~WriteLocker(){unLocker();}
private:
    ReadWriteLock* lock_;
};


COMMON_END_NAMESPACE

#endif