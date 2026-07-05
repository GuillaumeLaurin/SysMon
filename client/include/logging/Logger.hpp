#pragma once

#include "interfaces/ILogger.hpp"

#include "interfaces/IErrorDispatcher.hpp"

#include <spdlog/spdlog.h>

#include <memory>

/**
 * @file Logger.hpp
 * @brief Declares Logger, the ILogger implementation built on spdlog that
 *        also forwards Error/Fatal entries to the error dispatcher.
 */

/**
 * @brief ILogger implementation built on spdlog (console + rotating file).
 *        Error and Fatal entries are additionally forwarded to the error dispatcher.
 */
class Logger : public ILogger
{
public:
    /**
     * @brief Constructs the logger and its spdlog backend.
     * @param dispatcher Pipeline receiving Error/Fatal entries as ErrorRecords.
     */
    Logger(std::shared_ptr<IErrorDispatcher> dispatcher);

    /** @brief Default destructor. */
    ~Logger() override = default;

    /**
     * @brief Logs a debug-level message.
     * @param message Message to log.
     * @param ctx Call-site context (file, function, line).
     */
    void Debug(std::string_view message, const LogContext& ctx) noexcept override;

    /**
     * @brief Logs an info-level message.
     * @param message Message to log.
     * @param ctx Call-site context (file, function, line).
     */
    void Info(std::string_view message, const LogContext& ctx) noexcept override;

    /**
     * @brief Logs a warning-level message.
     * @param message Message to log.
     * @param ctx Call-site context (file, function, line).
     */
    void Warning(std::string_view message, const LogContext& ctx) noexcept override;

    /**
     * @brief Logs an error-level message and reports it to the dispatcher.
     * @param message Message to log.
     * @param ctx Call-site context (file, function, line).
     */
    void Error(std::string_view message, const LogContext& ctx) noexcept override;

    /**
     * @brief Logs a fatal-level message and reports it to the dispatcher.
     * @param message Message to log.
     * @param ctx Call-site context (file, function, line).
     */
    void Fatal(std::string_view message, const LogContext& ctx) noexcept override;

private:
    /**
     * @brief Builds an ErrorRecord from a log entry and dispatches it.
     * @param message Original log message.
     * @param ctx Call-site context (file, function, line).
     * @param category Category to assign to the resulting error record.
     * @param severity Severity to assign to the resulting error record.
     */
    void DispatcherLog(
        std::string_view  message,
        const LogContext& ctx,
        std::string       category,
        ErrorSeverity     severity
    ) noexcept;

    /** @brief Pipeline receiving Error/Fatal entries as ErrorRecords. */
    std::shared_ptr<IErrorDispatcher>    _Dispatcher;
    /** @brief Underlying spdlog logger instance. */
    std::shared_ptr<spdlog::logger>      _SpdLogger;
    /** @brief Rotating file sink used by the spdlog logger. */
    std::shared_ptr<spdlog::sinks::sink> _FileSink;
};
