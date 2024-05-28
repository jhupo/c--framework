#include <semaphore/semaphore.h>

#include <mutex>
#include <atomic>
#include <condition_variable>

COMMON_BEGIN_NAMESPACE

class SemaphoreImpl
{
public:
    SemaphoreImpl(int count, Semaphore* self);
    ~SemaphoreImpl();

    Semaphore*const             self_;
    std::mutex                  mutex_;
    std::condition_variable     cv_;
    std::atomic<int>            count_;
};

SemaphoreImpl::SemaphoreImpl(int count, Semaphore* self)
    : self_(self)
    , count_(count)
{

}

SemaphoreImpl::~SemaphoreImpl()
{
    count_ = 0;
    cv_.notify_all();
}

Semaphore::Semaphore(int count)
    : d(new SemaphoreImpl(count, this))
{

}

Semaphore::~Semaphore()
{
    delete d;
}

void Semaphore::wait()
{
    std::unique_lock<std::mutex> lock(d->mutex_);
    d->cv_.wait(lock, [this] { return d->count_ > 0; });
    --d->count_;
}

void Semaphore::notify()
{
    std::lock_guard<std::mutex> lock(d->mutex_);
    ++d->count_;
    d->cv_.notify_one();
}

bool Semaphore::try_wait()
{
    std::lock_guard<std::mutex> lock(d->mutex_);
    if (d->count_ > 0){
        --d->count_;
        return true;
    }
    return false;
}

bool Semaphore::timed_wait(std::chrono::milliseconds timeout)
{
    std::unique_lock<std::mutex> lock(d->mutex_);
    if (d->cv_.wait_for(lock, timeout, [this] { return d->count_ > 0; })){
        --d->count_;
        return true;
    }
    return false;
}


COMMON_END_NAMESPACE
