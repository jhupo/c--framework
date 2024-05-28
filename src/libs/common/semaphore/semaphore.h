#ifndef __COMMON_SEMAPHORE_H__
#define __COMMON_SEMAPHORE_H__

#include <common_global.h>

#include <chrono>

COMMON_BEGIN_NAMESPACE

class COMMON_EXPORT Semaphore
{
    DISABLE_COPY(Semaphore)
public:
    explicit Semaphore(int count);
    virtual ~Semaphore();

    void wait();

    void notify();

    bool try_wait();

    bool timed_wait(std::chrono::milliseconds timeout);

private:
    class SemaphoreImpl* d;
};

COMMON_END_NAMESPACE

#endif