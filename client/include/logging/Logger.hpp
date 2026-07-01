#pragma once

#include "interfaces/ILogger.hpp"

#include "interfaces/IErrorDispatcher.hpp"

#include <spdlog/spdlog.h>

#include <memory>

class Logger : public ILogger
{
public:
    Logger(std::shared_ptr<IErrorDispatcher> dispatcher);

    ~Logger() override = default;

    void Debug(std::string_view message, const LogContext& ctx) noexcept override;
    void Info(std::string_view message, const LogContext& ctx) noexcept override;
    void Warning(std::string_view message, const LogContext& ctx) noexcept override;
    void Error(std::string_view message, const LogContext& ctx) noexcept override;
    void Fatal(std::string_view message, const LogContext& ctx) noexcept override;

private:
    void DispatcherLog(
        std::string_view  message, 
        const LogContext& ctx, 
        std::string       category, 
        ErrorSeverity     severity
    ) noexcept;

    std::shared_ptr<IErrorDispatcher>    _Dispatcher;
    std::shared_ptr<spdlog::logger>      _SpdLogger;
    std::shared_ptr<spdlog::sinks::sink> _FileSink;
};