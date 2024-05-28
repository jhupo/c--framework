#include <thread/thread.h>
#include <semaphore/semaphore.h>

#include <thread>
#include <mutex>
#include <atomic>

COMMON_BEGIN_NAMESPACE

class ThreadImpl
{
public:
    ThreadImpl(Thread* self);
    ~ThreadImpl();

    void handler();

    Thread*const            self_;
    std::mutex              mutex_;
    std::atomic_bool        isRunning_;
    std::atomic_bool        isInterruptionRequested_;
    Semaphore               semaphore_;
    std::thread             thread_;
    std::function<void()>   cb_;
};

ThreadImpl::ThreadImpl(Thread* self)
    : self_(self)
    , isRunning_(false)
    , isInterruptionRequested_(false)
    , semaphore_(0)
{

}

ThreadImpl::~ThreadImpl()
{
    
}

Thread::Thread()
    : d(new ThreadImpl(this))
{

}

Thread::Thread(const std::function<void()>& cb)
    : d(new ThreadImpl(this))
{
    d->cb_ = cb;
}

Thread::~Thread()
{
    if(isRunning()){
        requestInterruption();
        wait();
    }
    delete d;
}

void Thread::start()
{
    std::unique_lock<std::mutex> lock(d->mutex_);
    if (d->isRunning_){
        return;
    }
    d->isInterruptionRequested_ = false;
    d->thread_ = std::thread(std::bind(&ThreadImpl::handler, d));
    d->semaphore_.wait();
}

void Thread::wait()
{
    std::unique_lock<std::mutex> lock(d->mutex_);
    if (d->isRunning_ || d->thread_.joinable()){
        d->thread_.join();
    }
}

bool Thread::isRunning()const
{
    return d->isRunning_;
}

bool Thread::isInterruptionRequested()
{
    return d->isInterruptionRequested_;
}

void Thread::requestInterruption()
{
    d->isInterruptionRequested_ = true;
}

void Thread::handler()
{
    
}

void ThreadImpl::handler()
{
    isRunning_ = true;
    semaphore_.notify();
    if(std::function<void()> cb = cb_ ? cb_ : std::bind(&Thread::handler, self_)){
        cb();
    }
    isRunning_ = false;
}



COMMON_END_NAMESPACE