#ifndef __COMMON_THREAD_H__
#define __COMMON_THREAD_H__

#include <common_global.h>

#include <functional>

COMMON_BEGIN_NAMESPACE

class COMMON_EXPORT Thread : public std::enable_shared_from_this<Thread>
{
    DISABLE_COPY(Thread)
public:
    Thread();
    explicit Thread(const std::function<void()>& cb);
    virtual~Thread();

    void start();

    void wait();

    bool isRunning() const;

    bool isInterruptionRequested();

    void requestInterruption();

protected:
    virtual void handler();
private:
    class ThreadImpl*       d;
    friend class ThreadImpl;
};


COMMON_END_NAMESPACE

#endif