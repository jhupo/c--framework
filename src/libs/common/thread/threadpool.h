#ifndef __COMMON_THREAD_POOL_H__
#define __COMMON_THREAD_POOL_H__

#include <common_global.h>

#include <functional>

COMMON_BEGIN_NAMESPACE

class COMMON_EXPORT ThreadPool
{
    DISABLE_COPY(ThreadPool)
public:
    typedef std::shared_ptr<ThreadPool> ptr;
    typedef std::function<void()> Task;

    ThreadPool();
    virtual~ThreadPool();

    void addTask(const Task& cb);
    bool tryStartTask(const Task& cb);
    void clear();

    uint64_t expiryTimeout() const;
    void setExpiryTimeout(uint64_t expiryTimeout);

    uint8_t maxThreadCount() const;
    void setMaxThreadCount(uint8_t maxThreadCount);

    uint8_t activeThreadCount() const;

    bool waitForDone(int msec = -1);

private:
    class ThreadPoolImpl* d;
};

COMMON_END_NAMESPACE

#endif