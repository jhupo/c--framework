#ifndef __FRAMEWORK_LOGGER_H__
#define __FRAMEWORK_LOGGER_H__

#include <framework_global.h>

#include <string>
#include <sstream>
#include <fstream>

#include <stdarg.h>

FRAMEWORK_BEGIN_NAMESPACE

namespace logger {

    enum class Level { Unknow, Debug, Info, Warning, Error, Fatal };

    class Logger;

    class Event
    {
        DISABLE_COPY(Event)
        DECLARE_SHARED_PTR(Event)
        DECLARE_CLASS_DETAIL(Event)
    public:
        explicit Event(const std::shared_ptr<Logger>& logger, Level level, const char* file, int line, const char* function, uint32_t elapse, uint32_t thread);

        Level level() const;

        const char* file() const;

        int line() const;

        const char* function() const;

        uint32_t elapse() const;

        uint32_t thread() const;

        std::string context() const;

        std::shared_ptr<Logger> logger() const;

        std::stringstream& ss() const;

        void format(const char* fmt, ...);

        void format(const char* fmt, va_list al);

    };

}

FRAMEWORK_END_NAMESPACE

#endif