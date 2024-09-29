#include <logger/logger.h>

FRAMEWORK_BEGIN_NAMESPACE

const char *ToString(Level level)
{
    switch (level)
    {
#define XX(name) \
    case name:   \
        return #name;
        XX(Debug)
        XX(Info)
        XX(Warning)
        XX(Error)
        XX(Fatal)
#undef XX
    default:
        break;
    }
    return "Unknow";
}

Level FromString(const char *str)
{
#define XX(name)                 \
    if (strcmp(#name, str) == 0) \
        return name;
    XX(Debug)
    XX(Info)
    XX(Warning)
    XX(Error)
    XX(Fatal)
#undef XX
    return Level::Unknow;
}

class event_impl
{
public:
    event_impl(const std::string &category, const event::source &src, level level, uint32_t thread, uint64_t time)
        : category_(category), src_(src), level_(level), thread_(thread), time_(time)
    {
    }

    std::string category_;
    event::source src_;
    level level_;
    uint32_t thread_;
    uint64_t time_;
    std::stringstream ss_;
};

event::event(const std::string &category, const source &src, level level, uint32_t thread, uint64_t time)
    : d(new event_impl(category, src, level, thread, time))
{
}

std::string event::category() const
{
    return d->category_;
}

const char *event::file() const
{
    return d->src_.file;
}

int event::line() const
{
    return d->src_.line;
}

const char *event::func() const
{
    return d->src_.func;
}

level event::level() const
{
    return d->level_;
}

uint32_t event::thread() const
{
    return d->thread_;
}

uint64_t event::time() const
{
    return d->time_;
}

std::string event::context() const
{
    return d->ss_.str();
}

std::stringstream &event::ss()
{
    return d->ss_;
}

void event::format(const char *fmt, ...)
{
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void event::format(const char *fmt, va_list al)
{
    char buf[1024];
    vsnprintf(buf, sizeof(buf), fmt, al);
    d->ss_ << buf;
}

FRAMEWORK_END_NAMESPACE