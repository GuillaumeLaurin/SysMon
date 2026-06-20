#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <string>

#include <spdlog/fmt/fmt.h>

struct LogContext
{
    const char* File;
    const char* Function;
    ULONG       Line;
};

class ILogger
{
public:
    virtual ~ILogger() = default;

    virtual void Debug(std::string_view message, const LogContext& ctx) noexcept = 0;
    virtual void Info(std::string_view message, const LogContext& ctx) noexcept = 0;
    virtual void Warning(std::string_view message, const LogContext& ctx) noexcept = 0;
    virtual void Error(std::string_view message, const LogContext& ctx) noexcept = 0;
    virtual void Fatal(std::string_view message, const LogContext& ctx) noexcept = 0;
};

#ifndef LOGGER_MACROS
#define LOGGER_MACROS

#define LOG_DEBUG(logger, msg, ...) \
    (logger).Debug(fmt::format(msg, ##__VA_ARGS__), { __FILE__, __FUNCTION__, __LINE__ })

#define LOG_INFO(logger, msg, ...) \
    (logger).Info(fmt::format(msg, ##__VA_ARGS__), { __FILE__, __FUNCTION__, __LINE__ })

#define LOG_WARNING(logger, msg, ...) \
    (logger).Warning(fmt::format(msg, ##__VA_ARGS__), { __FILE__, __FUNCTION__, __LINE__ })

#define LOG_ERROR(logger, msg, ...) \
    (logger).Error(fmt::format(msg, ##__VA_ARGS__), { __FILE__, __FUNCTION__, __LINE__ })

#define LOG_FATAL(logger, msg, ...) \
    (logger).Fatal(fmt::format(msg, ##__VA_ARGS__), { __FILE__, __FUNCTION__, __LINE__ })

#endif // !LOGGER_MACROS