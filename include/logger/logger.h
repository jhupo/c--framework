#ifndef __FRAMEWORK_LOGGER_H__
#define __FRAMEWORK_LOGGER_H__

#include <core.h>

#include <sstream>
#include <fstream>

#include <stdarg.h>

FRAMEWORK_BEGIN_NAMESPACE

enum class level
{
    Unknow,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class event;
class logger;

class formatter : public enable_disable_copy_from_this<formatter>
{
public:
    virtual ~formatter() = default;
    std::ostream &format(std::ostream &, const std::string &, level, const std::shared_ptr<event> &) = 0;
};

class appender : public enable_disable_copy_from_this<appender>
{
public:
    virtual ~appender() = default;
    virtual void log(const formatter::ptr &, const std::shared_ptr<logger> &, level, std::shared_ptr<event> &) = 0;
};

class event : public enable_disable_copy_from_this<event>
{
public:
    using ptr = std::shared_ptr<event>;

    struct source
    {
        source(const char *file, int line, const char *func)
            : file(file), line(line), func(func)
        {
        }
        const char *file;
        int line;
        const char *func;
    };

    explicit event(const std::string& category, const source &src, level level, uint32_t thread, uint64_t time);
    virtual ~event() { delete d; }

    std::string category() const;

    const char *file() const;
    int line() const;
    const char *func() const;

    level level() const;

    uint32_t thread() const;
    uint64_t time() const;

    std::string context() const;
    std::stringstream &ss();

    void format(const char *fmt, ...);
    void format(const char *fmt, va_list al);

private:
    class event_impl *d;
};

class logger : public enable_disable_copy_from_this<logger>, std::enable_shared_from_this<logger>
{
public:
    using ptr = std::shared_ptr<logger>;

    logger(const std::string &name = "default");
    virtual ~logger() { delete d; }

    void log(level level, const event::ptr &event);

    void debug(const event::ptr &event) { log(Debug, event); }

    void info(const event::ptr &event) { log(Info, event); }

    void warning(const event::ptr &event) { log(Warning, event); }

    void error(const event::ptr &event) { log(Error, event); }

    void fatal(const event::ptr &event)
    {
        log(Fatal, event);
        std::abort();
    }

    void setLevel(level level);

    level level() const;

    std::string name() const;

    void setAppender(const appender::ptr &appender);

    void setFormatter(const formatter::ptr &formatter);

private:
    class logger_impl *d;
};

FRAMEWORK_END_NAMESPACE

#endif