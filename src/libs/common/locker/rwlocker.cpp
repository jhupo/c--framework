#include <locker/rwlocker.h>

#include <mutex>
#include <atomic>
#include <condition_variable>

COMMON_BEGIN_NAMESPACE

class ReadWriteLockImpl
{
public:
    ReadWriteLockImpl(ReadWriteLock* self);
    ~ReadWriteLockImpl();

    ReadWriteLock*const     self_;
    std::atomic_uint16_t    readers_;
    std::atomic_bool        writer_;
    std::mutex              mutex_;
    std::condition_variable rcv_;
    std::condition_variable wcv_;
};

ReadWriteLockImpl::ReadWriteLockImpl(ReadWriteLock* self)
    : self_(self)
    , readers_(0)
    , writer_(false)
{

}

ReadWriteLockImpl::~ReadWriteLockImpl()
{
    writer_ = false;
    rcv_.notify_all();
    wcv_.notify_all();
}

ReadWriteLock::ReadWriteLock()
    : d(new ReadWriteLockImpl(this))
{

}

ReadWriteLock::~ReadWriteLock()
{
    delete d;
}

void ReadWriteLock::lockForRead()
{
    std::unique_lock<std::mutex> lock(d->mutex_);
    d->rcv_.wait(lock, [this] { return !d->writer_; });
    ++d->readers_;
}

void ReadWriteLock::lockForWrite()
{
    std::unique_lock<std::mutex> lock(d->mutex_);
    d->wcv_.wait(lock, [this] { return !d->writer_ && d->readers_ == 0; });
    d->writer_ = true;
}

void ReadWriteLock::unlock()
{
    std::lock_guard<std::mutex> lock(d->mutex_);
    if (d->writer_){
        d->writer_ = false;
        d->wcv_.notify_all();
    } else {
        --d->readers_;
        if (d->readers_ == 0){
            d->rcv_.notify_all();
        }
    }
}

bool ReadWriteLock::tryLockForRead()
{
    std::unique_lock<std::mutex> lock(d->mutex_);
    if (d->writer_){
        return false;
    }
    ++d->readers_;
    return true;
}

bool ReadWriteLock::tryLockForWrite()
{
    std::unique_lock<std::mutex> lock(d->mutex_);
    if (d->writer_ || d->readers_ > 0){
        return false;
    }
    d->writer_ = true;
    return true;
}


COMMON_END_NAMESPACE