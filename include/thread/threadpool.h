#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <commondefs.h>

#include <functional>

namespace framework{

    class RunnableThread;
    class ThreadPoolPrivate;

    class FRAMEWORK_EXPORT Runnable
    {
        DISABLE_COPY(Runnable)
        DECLARE_SHARED_PTR(Runnable)
    public:
        explicit Runnable(const std::function<void()>& cb);
        virtual~Runnable();
        inline bool operator==(const Runnable::ptr& other) const { return this == other.get(); }
    protected:
        void run();
    private:
        friend class RunnableThread;
        std::function<void()>   _cb;
    };

    class FRAMEWORK_EXPORT ThreadPool 
    {
        DISABLE_COPY(ThreadPool)
        DECLARE_PRIVATE(ThreadPool)
        DECLARE_SHARED_PTR(ThreadPool)
    public:
        ThreadPool();
        virtual ~ThreadPool();

        void addTask(const Runnable::ptr& task);

        bool tryStartTask(const Runnable::ptr& task);

        size_t expiryTimeout() const;
        void setExpiryTimeout(size_t expiryTimeout);

        size_t maxThreadCount() const;
        void setMaxThreadCount(size_t maxThreadCount);

        size_t activeThreadCount() const;

        void cancel(const Runnable::ptr& task);

        bool waitForDone(int msecs = -1);

        void clear();

    private:
        const std::shared_ptr<ThreadPoolPrivate>        d_ptr;
    };




}



#endif