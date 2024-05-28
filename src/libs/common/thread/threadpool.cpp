#include <thread/threadpool.h>
#include <thread/thread.h>
#include <logger/logger.h>

#include <mutex>
#include <vector>
#include <thread>
#include <atomic>
#include <algorithm>
#include <queue>
#include <condition_variable>
#include <chrono>

COMMON_BEGIN_NAMESPACE

class TaskThread : public Thread
{
public:
    typedef std::shared_ptr<TaskThread> ptr;
    TaskThread(ThreadPoolImpl* manager);
    virtual~TaskThread();
    virtual void handler();
    void registerThreadInactive();
    inline bool operator==(const TaskThread::ptr& other) const { return this == other.get(); }
    inline bool operator!=(const TaskThread::ptr& other) const { return this != other.get(); }
public:
    ThreadPoolImpl*const            manager_;
    std::condition_variable         cv_;
    ThreadPool::Task                cb_;
};

class ThreadPoolImpl
{
public:
    ThreadPoolImpl(ThreadPool* self);
    ~ThreadPoolImpl();

    size_t activeThreadCount();

    bool tooManyThreadsActive();

    void reset();

    bool waitForDone(int msec = -1);

    ThreadPool*const                    self_;
    std::mutex                          mutex_;
    std::condition_variable             cv_;
    std::vector<TaskThread::ptr>        threads_;
    std::vector<TaskThread::ptr>        waitThreads_;
    std::queue<ThreadPool::Task>        tasks_;
    std::atomic_bool                    isExiting_;
    std::atomic<uint16_t>               maxThreadCount_;
    std::atomic<uint16_t>               activeThreadCount_;
    uint64_t                            expiryTimeout_;
};

ThreadPoolImpl::ThreadPoolImpl(ThreadPool* self)
    : self_(self)
    , maxThreadCount_(std::thread::hardware_concurrency())
    , isExiting_(false)
    , activeThreadCount_(0)
    , expiryTimeout_(3000)
{

}

ThreadPoolImpl::~ThreadPoolImpl()
{
    
}

TaskThread::TaskThread(ThreadPoolImpl* manager)
    : manager_(manager)
    , cb_(nullptr)
{

}

TaskThread::~TaskThread()
{
    
}

void TaskThread::registerThreadInactive()
{
    if(--manager_->activeThreadCount_ == 0){
        manager_->cv_.notify_all();
    }
}

void TaskThread::handler()
{
    std::unique_lock<std::mutex> locker(manager_->mutex_);
    for(;;)
    {
        do{
            if(cb_){
                locker.unlock();
                try{
                    cb_();
                }catch(...){
                    LOG_ERROR("This is not supported, exceptions thrown in worker threads must be caught before control returns.");
                    registerThreadInactive();
                    throw;
                }
                locker.lock();
            }
            cb_ = nullptr;
            if(manager_->tooManyThreadsActive()){
                registerThreadInactive();
                break;
            }
            if(!manager_->tasks_.empty()){
                cb_ = manager_->tasks_.front();
                manager_->tasks_.pop();
            }
        }while(nullptr != cb_);
        if(manager_->isExiting_){
            registerThreadInactive();
            break;
        }
        bool expired = manager_->tooManyThreadsActive();
        if(!expired){
            manager_->waitThreads_.push_back(std::static_pointer_cast<TaskThread>(shared_from_this()));
            registerThreadInactive();
            cv_.wait_for(locker, std::chrono::milliseconds(manager_->expiryTimeout_));
            ++manager_->activeThreadCount_;
            std::vector<TaskThread::ptr>::iterator iter =
                    std::find(manager_->waitThreads_.begin(), manager_->waitThreads_.end(), std::static_pointer_cast<TaskThread>(shared_from_this()));
            if(iter != manager_->waitThreads_.end()){
                manager_->waitThreads_.erase(iter);
                expired = true;
            }
        }
        if(expired){
            registerThreadInactive();
            break;
        }
    }
}

size_t ThreadPoolImpl::activeThreadCount()
{
     return (threads_.size() - waitThreads_.size());
}

bool ThreadPoolImpl::tooManyThreadsActive()
{
    const size_t activeThreadCount = this->activeThreadCount();
    return activeThreadCount > maxThreadCount_ && activeThreadCount > 1;
}

ThreadPool::ThreadPool()
    : d(new ThreadPoolImpl(this))
{

}

ThreadPool::~ThreadPool()
{
    waitForDone();
    delete d;
}

void ThreadPool::addTask(const Task& cb)
{
    if(!tryStartTask(cb)){
        std::unique_lock<std::mutex> locker(d->mutex_);
        if(!d->waitThreads_.empty()){
            TaskThread::ptr thread = d->waitThreads_.back();
            d->waitThreads_.pop_back();
            thread->cb_ = cb;
            thread->cv_.notify_one();
        }else{
            d->tasks_.push(cb);
        }
    }
}

bool ThreadPool::tryStartTask(const Task& cb)
{
    std::unique_lock<std::mutex> locker(d->mutex_);
    if(d->isExiting_){
        return false;
    }
    if(d->tooManyThreadsActive()){
        return false;
    }   
    if(!d->waitThreads_.empty()){
        TaskThread::ptr thread = d->waitThreads_.back();
        d->waitThreads_.pop_back();
        thread->cb_ = cb;
        thread->cv_.notify_one();
        return true;
    }
    TaskThread::ptr thread(new TaskThread(d));
    thread->cb_ = cb;
    thread->start();
    d->threads_.push_back(thread);
    ++d->activeThreadCount_;
    return true;
}

void ThreadPool::clear()
{
    std::unique_lock<std::mutex> locker(d->mutex_);
    d->tasks_ = std::queue<Task>();
}

uint64_t ThreadPool::expiryTimeout() const
{
    return d->expiryTimeout_;
}

void ThreadPool::setExpiryTimeout(uint64_t expiryTimeout)
{
    std::unique_lock<std::mutex> locker(d->mutex_);
    if(d->expiryTimeout_ != expiryTimeout){
        d->expiryTimeout_ = expiryTimeout;
    }
}

uint8_t ThreadPool::maxThreadCount() const
{
    return d->maxThreadCount_;
}

void ThreadPool::setMaxThreadCount(uint8_t maxThreadCount)
{
    std::unique_lock<std::mutex> locker(d->mutex_);
    if(d->maxThreadCount_ != maxThreadCount){
        d->maxThreadCount_ = maxThreadCount;
        locker.unlock();
        while (!d->tasks_.empty() && tryStartTask(d->tasks_.front())){
            locker.lock();
            d->tasks_.pop();
            locker.unlock();
        }
    }
}

uint8_t ThreadPool::activeThreadCount() const
{
    std::unique_lock<std::mutex> locker(d->mutex_);
    return d->activeThreadCount();
}

bool ThreadPoolImpl::waitForDone(int msec)
{
    std::unique_lock<std::mutex> locker(mutex_);
    if(0 > msec){
        while (!(tasks_.empty() && activeThreadCount_ == 0)){
            cv_.wait(locker);
        }
    }else{
        auto start_time = std::chrono::high_resolution_clock::now();
        int t;
        while (!tasks_.empty() && activeThreadCount_ > 0)
        {
            auto current_time = std::chrono::high_resolution_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
            t = msec - elapsed_ms;
            if(t > 0){
                cv_.wait_for(locker, std::chrono::milliseconds(t));
            }else{
                break;
            }
        }
    }
    return tasks_.empty() && activeThreadCount_ == 0;
}

void ThreadPoolImpl::reset()
{
    std::unique_lock<std::mutex> locker(mutex_);
    isExiting_ = true;
    while(!threads_.empty())
    {
        std::vector<TaskThread::ptr> allThreadsCopy;
        allThreadsCopy.swap(threads_);
        locker.unlock();
        std::vector<TaskThread::ptr>::const_iterator iter = allThreadsCopy.begin();
        for(;iter != allThreadsCopy.end(); ++iter) {
            TaskThread::ptr thread = *iter;
            thread->cv_.notify_all();
            thread->wait();
        }
        locker.lock();
    }
    waitThreads_.clear();
    isExiting_ = false;
}

bool ThreadPool::waitForDone(int msec)
{
    bool rc = d->waitForDone(msec);
    if (rc){
        d->reset();
    }
    return rc;
}

COMMON_END_NAMESPACE

