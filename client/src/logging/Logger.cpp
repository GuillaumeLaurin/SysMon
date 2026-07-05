#include "logging/Logger.hpp"

#include "exceptions/SysMonException.hpp"

#include <spdlog/sinks/basic_file_sink.h>

/**
 * @file Logger.cpp
 * @brief Implements Logger: spdlog-backed logging plus forwarding of
 *        Error/Fatal entries to the error dispatcher.
 */

/** @brief Constructs the logger and its spdlog backend. */
Logger::Logger(std::shared_ptr<IErrorDispatcher> dispatcher)
    : _Dispatcher(dispatcher)
{
    _FileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        "logs/sysmon.log", false
    );
    _FileSink->set_level(spdlog::level::debug);

    std::vector<spdlog::sink_ptr> sinks = {_FileSink};
    _SpdLogger = std::make_shared<spdlog::logger>("SysMon", sinks.begin(), sinks.end());
    _SpdLogger->set_level(spdlog::level::debug);
    _SpdLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
}

/** @brief Logs a debug-level message. */
void Logger::Debug(std::string_view message, const LogContext& /*ctx*/) noexcept
{
    _SpdLogger->debug(message);
    _SpdLogger->flush();
}

/** @brief Logs an info-level message. */
void Logger::Info(std::string_view message, const LogContext& /*ctx*/) noexcept
{
    _SpdLogger->info(message);
    _SpdLogger->flush();
}

/** @brief Logs a warning-level message. */
void Logger::Warning(std::string_view message, const LogContext& /*ctx*/) noexcept
{
    _SpdLogger->warn(message);
    _SpdLogger->flush();
}

/** @brief Logs an error-level message and reports it to the dispatcher. */
void Logger::Error(std::string_view message, const LogContext& ctx) noexcept
{
    _SpdLogger->error(message);
    _SpdLogger->flush();
    if (_Dispatcher)
    {
        DispatcherLog(message, ctx, "ErrorLog", ErrorSeverity::Error);
    }
}

/** @brief Logs a fatal-level message and reports it to the dispatcher. */
void Logger::Fatal(std::string_view message, const LogContext& ctx) noexcept
{
    _SpdLogger->critical(message);
    _SpdLogger->flush();
    if (_Dispatcher)
    {
        DispatcherLog(message, ctx, "FatalLog", ErrorSeverity::Fatal);
    }
}

/** @brief Builds an ErrorRecord from a log entry and dispatches it. */
void Logger::DispatcherLog(
    std::string_view  message, 
    const LogContext& ctx, 
    std::string       category, 
    ErrorSeverity     severity
) noexcept
{
    LARGE_INTEGER ft;
    GetSystemTimeAsFileTime((FILETIME*)&ft);

    ErrorRecord record;

    record.UptimeMs     = GetTickCount64();
    record.Uuid         = std::to_string(record.UptimeMs);
    record.Timestamp    = ft;
    record.Severity     = severity;
    record.Category     = category;
    record.UserId       = SysMonException::CaptureUserId();
    record.MemoryUsedKb = SysMonException::CaptureMemoryUsedKb();
    // context
    record.Context.ModuleName      = "";
    record.Context.Message         = message;
    record.Context.Type            = "Log";
    record.Context.Action          = "";
    record.Context.File            = ctx.File;
    record.Context.Function        = ctx.Function;
    record.Context.Line            = ctx.Line;
    record.Context.Pid             = GetCurrentProcessId();
    record.Context.ThreadId        = GetCurrentThreadId();
    record.Context.FaultingAddress = 0;
    record.Context.Stacktrace      = SysMonException::CaptureStackTrace();

    record.Fingerprint = ErrorRecord::ComputeFingerprint(record);

    _Dispatcher->Dispatch(record);
}