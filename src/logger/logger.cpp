#include <logger/logger.h>


FRAMEWORK_BEGIN_NAMESPACE

namespace logger {

    const char* ToString(Level level)
    {

    }

    Level FromString(const char* str)
    {

    }


    class Event::Impl
    {
    public:
        Impl(const std::shared_ptr<Logger>& logger, Level level, const char* file, int line, const char* function, uint32_t elapse, uint32_t thread)
            : logger_(logger)
            , level_(level)
            , file_(file)
            , line_(line)
            , function_(function)
            , elapse_(elapse)
            , thread_(thread)
        {

        }
        std::shared_ptr<Logger> logger_;
        Level                   level_;
        const char*             file_;
        int                     line_;
        const char*             function_;
        uint32_t                elapse_;
        uint32_t                thread_;
        std::stringstream       ss_;
    };


    Event::Event(const std::shared_ptr<Logger>& logger, Level level, const char* file, int line, const char* function, uint32_t elapse, uint32_t thread)
        : d(new Impl(logger, level, file, line, function, elapse, thread))
    {

    }

    Level Event::level() const
    {
        return d->level_;
    }

    const char* Event::file() const
    {
        return d->file_;
    }

    int Event::line() const
    {
        return d->line_;
    }

    const char* Event::function() const
    {
        return d->function_;
    }

    uint32_t Event::elapse() const
    {
        return d->elapse_;
    }

    uint32_t Event::thread() const
    {
        return d->thread_;
    }

    std::string Event::context() const
    {
        return d->ss_.str();
    }

    std::shared_ptr<Logger> Event::logger() const
    {
        return d->logger_;
    }

    std::stringstream& Event::ss() const
    {
        return d->ss_;
    }

    void Event::format(const char* fmt, ...)
    {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }

    void Event::format(const char* fmt, va_list al)
    {
        char buffer[1024] = { 0 };
        vsnprintf(buffer, sizeof(buffer), fmt, al);
        d->ss_ << buffer;
    }


}

FRAMEWORK_END_NAMESPACE