#include <thread/threadpool.h>

#include <set>
#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <condition_variable>

namespace framework{

    class RunnableThread : public std::enable_shared_from_this<RunnableThread>
    {
        DISABLE_COPY(RunnableThread)
        DECLARE_SHARED_PTR(RunnableThread)
    public:
        explicit RunnableThread(ThreadPoolPrivate* manager);
        ~RunnableThread();

        inline bool joinable()const { return _thread.joinable(); }
        inline void join() { _thread.join(); }

        void run();

        void registerThreadInactive();

        inline bool operator==(const RunnableThread::ptr other) const { return this == other.get(); }

        ThreadPoolPrivate*const         _manager;
        Runnable::ptr                   _runnable;
        std::condition_variable         _variable;
        std::thread                     _thread;
    };

    class ThreadPoolPrivate
    {
        DECLARE_PUBLIC(ThreadPool)
    public:

        ThreadPoolPrivate(ThreadPool* self);
        ~ThreadPoolPrivate();

        bool tooManyThreadsActive()const;
        size_t activeThreadCount()const;
        bool waitForDone(int msec);
        void reset();
        void clear();
        void startThread(const Runnable::ptr& task);
        bool tryStartTask(const Runnable::ptr& task);
        void tryToStartMoreThreads();
        bool stealRunnable(const Runnable::ptr&);

        ThreadPool*const                                                    s_ptr;
        std::mutex                                                          _mutex;
        std::condition_variable                                             _variable;
        std::set<RunnableThread::ptr>                                       _allThreads;
        std::queue<RunnableThread::ptr>                                     _waitingThreads;
        std::queue<RunnableThread::ptr>                                     _expiredThreads;
        std::vector<Runnable::ptr>                                          _tasks;
        bool                                                                _exit;
        size_t                                                              _maxThreadCount;
        size_t                                                              _activeThreads;
        size_t                                                              _expiryTimeout;
        size_t                                                              _reservedThreads;
    };

    RunnableThread::RunnableThread(ThreadPoolPrivate* manager)
        : _manager(manager)
    {

    }

    RunnableThread::~RunnableThread()
    {
        if(_thread.joinable()){
            _thread.join();
        }
    }

    void RunnableThread::run()
    {
        std::unique_lock<std::mutex> locker(_manager->_mutex);
        for(;;){
            Runnable::ptr r = _runnable;
            _runnable.reset();
            do{
                if(r){
                    locker.unlock();
                    try{
                        r->run();
                    }
                    catch(...){
                        std::cerr << "This is not supported, exceptions thrown in worker threads must be caught before control returns.";
                        registerThreadInactive();
                        throw;
                    }
                    locker.lock();
                }
                if(_manager->tooManyThreadsActive()){
                    break;
                }
                if(!_manager->_tasks.empty()){
                    r = _manager->_tasks.front();
                    _manager->_tasks.erase(_manager->_tasks.begin());
                }else{
                    r.reset();
                }
            }while(NULL != r);
            if(_manager->_exit){
                registerThreadInactive();
                break;
            }
            bool expired = _manager->tooManyThreadsActive();
            if(!expired){
                _manager->_waitingThreads.push(shared_from_this());
                registerThreadInactive();
                _variable.wait_for(locker,std::chrono::milliseconds(_manager->_expiryTimeout));
                ++_manager->_activeThreads;
                std::queue<RunnableThread::ptr> waitingThreadsCopy;
                while(!_manager->_waitingThreads.empty()){
                    RunnableThread::ptr thread = _manager->_waitingThreads.front();
                    _manager->_waitingThreads.pop();
                    if(thread == shared_from_this()){
                        expired = true;
                        continue;
                    }
                    waitingThreadsCopy.push(thread);
                }
                _manager->_waitingThreads.swap(waitingThreadsCopy);
            }
            if(expired){
                _manager->_expiredThreads.push(shared_from_this());
                registerThreadInactive();
                break;
            }
        }
    }

    void RunnableThread::registerThreadInactive()
    {
        if (--_manager->_activeThreads == 0)
            _manager->_variable.notify_all();
    }

    ThreadPoolPrivate::ThreadPoolPrivate(ThreadPool* self)
        : s_ptr(self)
        , _exit(false)
        , _maxThreadCount(std::thread::hardware_concurrency())
        , _activeThreads(0)
        , _expiryTimeout(30000)
        , _reservedThreads(0)
    {

    }

    ThreadPoolPrivate::~ThreadPoolPrivate()
    {
        
    }

    bool ThreadPoolPrivate::tooManyThreadsActive()const
    {
        const size_t count = activeThreadCount();
        return count > _maxThreadCount && (count - _reservedThreads) > 1;
    }

    size_t ThreadPoolPrivate::activeThreadCount()const
    {
        return (_allThreads.size()
                - _expiredThreads.size()
                - _waitingThreads.size()
                + _reservedThreads);
    }

    bool ThreadPoolPrivate::waitForDone(int msec)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if(_tasks.empty() && _waitingThreads.empty()){
            return true;
        }
        if(msec < 0){
            _variable.wait(lock,[this]{
                return _tasks.empty() && _waitingThreads.empty();
            });
            return true;
        }
        const auto timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(msec);
        while(std::chrono::steady_clock::now() < timeout){
            if(_tasks.empty() && _waitingThreads.empty()){
                return true;
            }
            _variable.wait_until(lock,timeout);
        }
        return _tasks.empty() && _waitingThreads.empty();
    }

    void ThreadPoolPrivate::reset()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _exit = true;
        while(!_allThreads.empty())
        {
            std::set<RunnableThread::ptr> allThreadCopy;
            allThreadCopy.swap(_allThreads);
            lock.unlock();
            for(const auto& thread : allThreadCopy)
            {
                thread->_variable.notify_one();
                if(thread->joinable()){
                    thread->join();
                }
            }
            lock.lock();
        }
        _waitingThreads = std::queue<RunnableThread::ptr>();
        _expiredThreads = std::queue<RunnableThread::ptr>();
        _exit = false;
    }

    void ThreadPoolPrivate::clear()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.clear();
    }

    void ThreadPoolPrivate::startThread(const Runnable::ptr& task)
    {
        RunnableThread::ptr thread = std::make_shared<RunnableThread>(this);
        thread->_runnable = task;
        thread->_thread = std::thread(&RunnableThread::run,thread.get());
        _allThreads.insert(thread);
        ++_activeThreads;
    }

    bool ThreadPoolPrivate::tryStartTask(const Runnable::ptr& task)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if(_allThreads.empty()){
            startThread(task);
            return true;
        }
        if (activeThreadCount() >= _maxThreadCount){
            return false;
        }
        if (!_waitingThreads.empty()) {
            RunnableThread::ptr thread = _waitingThreads.front();
            _waitingThreads.pop();
            thread->_runnable = task;
            thread->_variable.notify_one();
            return true;
        }
        if(!_expiredThreads.empty()){
            RunnableThread::ptr thread = _expiredThreads.front();
            _expiredThreads.pop();
            _allThreads.erase(thread);
        }
        startThread(task);
        return true;
    }

    void ThreadPoolPrivate::tryToStartMoreThreads()
    {
        while (!_tasks.empty() && tryStartTask(_tasks.front())){
            _tasks.erase(_tasks.begin());
        }
    }

    bool ThreadPoolPrivate::stealRunnable(const Runnable::ptr& task)
    {
        std::unique_lock<std::mutex> locker(_mutex);
        const auto& iter = std::find(_tasks.begin(),_tasks.end(),task);
        if(iter != _tasks.end()){
            _tasks.erase(iter);
            return true;
        }
        return false;
    }

    ThreadPool::ThreadPool()
        : d_ptr(new ThreadPoolPrivate(this))
    {

    }

    ThreadPool::~ThreadPool()
    {
        waitForDone();
    }

    void ThreadPool::addTask(const Runnable::ptr& task)
    {
        DATA_PTR(ThreadPool);
        std::unique_lock<std::mutex> locker(d->_mutex);
        if(d->_tasks.end() != std::find(d->_tasks.begin(),d->_tasks.end(),task)){
            return;
        }
        if(!d->tryStartTask(task)){
            d->_tasks.push_back(task);
        }
    }

    bool ThreadPool::tryStartTask(const Runnable::ptr& task)
    {
        DATA_PTR(ThreadPool);
        std::unique_lock<std::mutex> locker(d->_mutex);
        if(!d->_allThreads.empty() && d->activeThreadCount() >= d->_maxThreadCount){
            return false;
        }
        return d->tryStartTask(task);
    }

    size_t ThreadPool::expiryTimeout() const
    {
        DATA_PTR(const ThreadPool);
        return d->_expiryTimeout;
    }

    void ThreadPool::setExpiryTimeout(size_t expiryTimeout)
    {
        DATA_PTR(ThreadPool);
        d->_expiryTimeout = expiryTimeout;
    }

    size_t ThreadPool::maxThreadCount() const
    {
        DATA_PTR(const ThreadPool);
        return d->_maxThreadCount;
    }

    void ThreadPool::setMaxThreadCount(size_t maxThreadCount)
    {
        DATA_PTR(ThreadPool);
        if(d->_maxThreadCount != maxThreadCount){
            d->_maxThreadCount = maxThreadCount;
            d->tryToStartMoreThreads();
        }
    }

    size_t ThreadPool::activeThreadCount() const
    {
        DATA_PTR(const ThreadPool);
        return d->activeThreadCount();
    }

    void ThreadPool::cancel(const Runnable::ptr& task)
    {
        DATA_PTR(ThreadPool);
        d->stealRunnable(task);
    }

    bool ThreadPool::waitForDone(int msecs)
    {
        DATA_PTR(ThreadPool);
        bool rc = d->waitForDone(msecs);
        if(rc){
            d->reset();
        }
        return rc;
    }

    void ThreadPool::clear()
    {
        DATA_PTR(ThreadPool);
        d->clear();
    }

    Runnable::Runnable(const std::function<void()>& cb)
        : _cb(cb)
    {

    }

    Runnable::~Runnable()
    {

    }

    void Runnable::run()
    {
        if(_cb){
            _cb();
        }
    }

}