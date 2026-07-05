/**
 * @file ILogger.hpp
 * @brief Contract for the leveled application logger and its LOG_* helper macros.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <string>

#include <spdlog/fmt/fmt.h>

/**
 * @brief Source location captured with every log entry.
 */
struct LogContext
{
    const char* File;     ///< Source file emitting the log
    const char* Function; ///< Function emitting the log
    ULONG       Line;     ///< Line number in the source file
};

/**
 * @brief Leveled application logger. Use the LOG_* macros below so the
 *        source location is captured automatically.
 *
 * @note Implementations are expected to be thread-safe, since the LOG_* macros
 *       may be invoked concurrently from any worker thread.
 */
class ILogger
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~ILogger() = default;

    /**
     * @brief Logs a debug-level message.
     * @param message Message text to log.
     * @param ctx Source location captured at the call site.
     */
    virtual void Debug(std::string_view message, const LogContext& ctx) noexcept = 0;

    /**
     * @brief Logs an info-level message.
     * @param message Message text to log.
     * @param ctx Source location captured at the call site.
     */
    virtual void Info(std::string_view message, const LogContext& ctx) noexcept = 0;

    /**
     * @brief Logs a warning-level message.
     * @param message Message text to log.
     * @param ctx Source location captured at the call site.
     */
    virtual void Warning(std::string_view message, const LogContext& ctx) noexcept = 0;

    /**
     * @brief Logs an error-level message; also reported to the error dispatcher.
     * @param message Message text to log.
     * @param ctx Source location captured at the call site.
     */
    virtual void Error(std::string_view message, const LogContext& ctx) noexcept = 0;

    /**
     * @brief Logs a fatal-level message; also reported to the error dispatcher.
     * @param message Message text to log.
     * @param ctx Source location captured at the call site.
     */
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
