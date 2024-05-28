#include <timer/timermanager.h>
#include <thread/thread.h>
#include <thread/threadpool.h>
#include <locker/rwlocker.h>

#include <chrono>
#include <set>
#include <mutex>
#include <atomic>
#include <condition_variable>

COMMON_BEGIN_NAMESPACE

namespace{
    uint64_t GetCurrentMS()
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }
}

class TimerThread : public Thread
{
public:
    TimerThread(TimerManagerImpl* manager);
    virtual~TimerThread() = default;
protected:
    virtual void handler();
private:
    TimerManagerImpl*const      manager_;
};

class TimerManagerImpl
{
public:
    TimerManagerImpl(TimerManager *q);
    ~TimerManagerImpl();

    bool detectClockRollover(uint64_t now_ms);

    void addTimer(Timer::ptr timer);

    uint64_t nextTimer();

    bool hasTimer();

    void listExpiredCb(std::vector<std::function<void()> >& cbs);

    TimerManager *const                         self_;
    std::mutex                                  mutex_;
    std::condition_variable                     cv_;
    std::set<Timer::ptr, Timer::Comparator>     timers_;
    bool                                        tickled_;
    uint64_t                                    previouseTime_;
    std::atomic_bool                            exit_;
    TimerThread                                 thread_;
};

class TimerImpl
{
public:
    TimerImpl(Timer *self);
    ~TimerImpl();

    Timer *const                    self_;
    bool                            loop_;
    uint64_t                        ms_;
    uint64_t                        next_;
    std::function<void()>           cb_;
    TimerManagerImpl*               manager_;
};

TimerThread::TimerThread(TimerManagerImpl* manager)
    : manager_(manager)
{

}

void TimerThread::handler()
{
    ThreadPool pool;
    while(!isInterruptionRequested())
    {
        {
            std::unique_lock<std::mutex> locker(manager_->mutex_);
            if(!manager_->hasTimer()){
                manager_->cv_.wait(locker);
            }
            uint64_t now = manager_->nextTimer();
            manager_->cv_.wait_for(locker, std::chrono::milliseconds(now));
        }
        std::vector<std::function<void()> > cbs;
        manager_->listExpiredCb(cbs);
        for(auto& cb : cbs){
            pool.addTask(cb);
        }
    }
    pool.clear();
    pool.waitForDone();
}

TimerManagerImpl::TimerManagerImpl(TimerManager *q)
    : self_(q)
    , tickled_(false)
    , previouseTime_(GetCurrentMS())
    , exit_(false)
    , thread_(this)
{
    thread_.start();
}

TimerManagerImpl::~TimerManagerImpl()
{
    if(thread_.isRunning()){
        thread_.requestInterruption();
        thread_.wait();
    }
}

bool TimerManagerImpl::detectClockRollover(uint64_t now_ms)
{
    bool rollover = false;
        if(now_ms < previouseTime_ &&
        now_ms < (previouseTime_ - 60 * 60 * 1000)) {
        rollover = true;
    }
    previouseTime_ = now_ms;
    return rollover;
}

void TimerManagerImpl::addTimer(Timer::ptr timer)
{
    std::set<Timer::ptr, Timer::Comparator>::iterator it = 
        timers_.insert(timer).first;
    bool at_front = (it == timers_.begin()) && !tickled_;
    if(at_front){
        tickled_ = true;
    }
    if(at_front){
        cv_.notify_one();
    }
}

bool TimerManagerImpl::hasTimer()
{
    return !timers_.empty();
}

void TimerManagerImpl::listExpiredCb(std::vector<std::function<void()> >& cbs)
{
    std::unique_lock<std::mutex> locker(mutex_);
    uint64_t now_ms = GetCurrentMS();
    std::vector<Timer::ptr> expired;
    if(timers_.empty()){
        return;
    }
    bool rollover = detectClockRollover(now_ms);
    if(!rollover && ((*timers_.begin())->d->next_ > now_ms)) {
        return;
    }
    Timer::ptr now_timer(new Timer(now_ms));
    auto it = rollover ? timers_.end() : timers_.lower_bound(now_timer);
    while(it != timers_.end() && (*it)->d->next_ == now_ms) {
        ++it;
    }
    expired.insert(expired.begin(), timers_.begin(), it);
    timers_.erase(timers_.begin(), it);
    cbs.reserve(expired.size());

    for(auto& timer : expired) {
        cbs.push_back(timer->d->cb_);
        if(timer->d->loop_) {
            timer->d->next_ = now_ms + timer->d->ms_;
            timers_.insert(timer);
        } else {
            timer->d->cb_ = nullptr;
        }
    }
}

uint64_t TimerManagerImpl::nextTimer()
{
    if(timers_.empty()){
        return ~0ull;
    }
    const Timer::ptr &timer = *timers_.begin();
    uint64_t now = GetCurrentMS();
    if(now >= timer->d->next_){
        return 0;
    } else {
        return timer->d->next_ - now;
    }
}

TimerImpl::TimerImpl(Timer *self)
    : self_(self)
    , loop_(false)
    , ms_(0)
    , next_(0)
    , manager_(nullptr)
{

}

TimerImpl::~TimerImpl()
{

}

bool Timer::Comparator::operator()(const Timer::ptr &lhs, const Timer::ptr &rhs) const
{
    if (!lhs && !rhs){
        return false;
    }
    if (!lhs){
        return true;
    }
    if (!rhs){
        return false;
    }
    if (lhs->d->next_ < rhs->d->next_){
        return true;
    }
    if (rhs->d->next_ < lhs->d->next_){
        return false;
    }
    return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, std::function<void()> &cb, bool loop, TimerManagerImpl *manager)
    : d(new TimerImpl(this))
{
    d->ms_ = ms;
    d->cb_ = cb;
    d->loop_ = loop;
    d->manager_ = manager;
    d->next_ = GetCurrentMS() + ms;
}

Timer::Timer(uint64_t next)
    : d(new TimerImpl(this))
{
    d->next_ = next;
}

Timer::~Timer()
{
    delete d;
}

bool Timer::cancel()
{
    std::unique_lock<std::mutex> locker(d->manager_->mutex_);
    if(d->cb_){
        d->cb_ = NULL;
        std::set<Timer::ptr, Timer::Comparator>::const_iterator iter = d->manager_->timers_.find(shared_from_this());
        d->manager_->timers_.erase(iter);
        return true;
    }
    return false;
}   

bool Timer::refresh()
{
    std::unique_lock<std::mutex> locker(d->manager_->mutex_);
    if(!d->cb_){
        return false;
    }
    std::set<Timer::ptr, Timer::Comparator>::const_iterator iter = d->manager_->timers_.find(shared_from_this());
    if(iter == d->manager_->timers_.end()){
        return false;
    }
    d->manager_->timers_.erase(iter);
    d->next_ = GetCurrentMS() + d->ms_;
    d->manager_->timers_.insert(shared_from_this());
    return true;
}

bool Timer::reset(uint64_t ms, bool from_now)
{
    if(ms == d->ms_ && !from_now){
        return false;
    }
    std::unique_lock<std::mutex> locker(d->manager_->mutex_);
    if(!d->cb_){
        return false;
    }
    std::set<Timer::ptr, Timer::Comparator>::const_iterator iter = d->manager_->timers_.find(shared_from_this());
    if(iter == d->manager_->timers_.end()){
        return false;
    }
    d->manager_->timers_.erase(iter);
    uint64_t start = 0;
    if(from_now) {
        start = GetCurrentMS();
    } else {
        start = d->next_ - d->ms_;
    }
    d->ms_ = ms;
    d->next_ = start + d->ms_;
    d->manager_->addTimer(shared_from_this());
    return true;
}

TimerManager::TimerManager()
    : d(new TimerManagerImpl(this))
{

}

TimerManager::~TimerManager()
{
    delete d;
}

TimerManager* TimerManager::inst()
{
    static TimerManager::ptr inst_(new TimerManager);
    return inst_.get();
}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool loop)
{
    std::unique_lock<std::mutex> locker(d->mutex_);
    Timer::ptr timer(new Timer(ms, cb, loop, d));
    d->addTimer(timer);
    return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) 
{
    std::shared_ptr<void> tmp = weak_cond.lock();
    if(tmp) {
        cb();
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> cond, bool loop)
{
    return addTimer(ms, std::bind(&OnTimer, cond, cb), loop);
}

void TimerManager::clear()
{
    std::unique_lock<std::mutex> locker(d->mutex_);
    d->timers_.clear();
}

COMMON_END_NAMESPACE