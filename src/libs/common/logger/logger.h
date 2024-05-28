#ifndef __COMMON_LOGGER_H__
#define __COMMON_LOGGER_H__

#include <common_global.h>

#include <spdlog/spdlog.h>

COMMON_BEGIN_NAMESPACE

namespace logger{

    bool COMMON_EXPORT init_logger(const std::string& config, const std::string& channel = "default");

    #define LOGGING_REGISTER_CATEGORY(Channel) spdlog::register_logger(std::make_shared<spdlog::logger>(\
                                                                            Channel,\
                                                                            spdlog::default_logger_raw()->sinks().begin(),\
                                                                            spdlog::default_logger_raw()->sinks().end()))
    #define DECLARE_LOGGING_CATEGORY(Channel) \
            extern const std::shared_ptr<spdlog::logger> name();

    #define LOGGING_CATEGORY(name,model)                                                \
            const std::shared_ptr<spdlog::logger> name()                                \
            {                                                                           \
                if(spdlog::get(model)){                                                 \
                    return spdlog::get(model);                                          \
                }                                                                       \
                LOGGING_REGISTER_CATEGORY(model);                                       \
                return spdlog::get(model);                                              \
            }

#define LOGGER_DEBUG(logger,...) SPDLOG_LOGGER_DEBUG(logger(),__VA_ARGS__)
#define LOGGER_INFO(logger,...) SPDLOG_LOGGER_INFO(logger(),__VA_ARGS__)
#define LOGGER_WARN(logger,...) SPDLOG_LOGGER_WARN(logger(),__VA_ARGS__)
#define LOGGER_ERROR(logger,...) SPDLOG_LOGGER_ERROR(logger(),__VA_ARGS__)
#define LOGGER_CRITICAL(logger,...) SPDLOG_LOGGER_CRITICAL(logger(),__VA_ARGS__)

#define LOG_DEBUG(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

}

COMMON_END_NAMESPACE

#endif