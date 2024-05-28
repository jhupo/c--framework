#ifndef __COMMON_TIMER_MANAGER_H__
#define __COMMON_TIMER_MANAGER_H__

#include <common_global.h>

#include <vector>
#include <functional>

COMMON_BEGIN_NAMESPACE

class TimerManagerImpl;

class COMMON_EXPORT Timer : public std::enable_shared_from_this<Timer>
{
    DISABLE_COPY(Timer)
public:
    typedef std::shared_ptr<Timer> ptr;
    struct Comparator
    {
        bool operator()(const Timer::ptr &lhs, const Timer::ptr &rhs) const;
    };
    bool cancel();
    bool refresh();
    bool reset(uint64_t ms, bool from_now);
    inline bool operator==(const Timer::ptr& other) const{
        return this == other.get();
    }

    inline bool operator!=(const Timer::ptr& other) const{
        return this != other.get();
    }
    ~Timer();
private:
    Timer(uint64_t ms, std::function<void()> &cb, bool loop, TimerManagerImpl *manager);
    Timer(uint64_t next);
private:
    friend class TimerManager;
    friend class TimerManagerImpl;
    class TimerImpl*    d;
};

class COMMON_EXPORT TimerManager 
{
    DISABLE_COPY(TimerManager)
public:
    typedef std::shared_ptr<TimerManager> ptr;
    TimerManager();
    virtual~TimerManager();

    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool loop = true);

    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> cond, bool loop = false);

    void clear();

    static TimerManager* inst();

private:
    class TimerManagerImpl* d;
};

COMMON_END_NAMESPACE

#endif