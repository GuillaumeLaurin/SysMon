#include "exceptions/ExceptionHandler.hpp"

#include "reporter/ErrorDispatcher.hpp"

#include "reporter/ConsoleSink.hpp"

#include "reporter/JsonFormatter.hpp"

#include <typeinfo>

ExceptionHandler::ExceptionHandler(std::shared_ptr<IErrorDispatcher> dispatcher)
    : _ErrorDispatcher(dispatcher != nullptr ? dispatcher : std::make_shared<ErrorDispatcher>())
{
    if (dispatcher == nullptr)
    {
        auto sink = std::make_shared<ConsoleSink>("default", stderr);
        auto formatter = std::make_shared<JsonFormatter>();

        _ErrorDispatcher->AddSink(sink);
        _ErrorDispatcher->SetFormatter(formatter);
    }
}

void ExceptionHandler::Handle(const SysMonException& exception) noexcept
{
    _ErrorDispatcher->Dispatch(exception.ToRecord());
}

void ExceptionHandler::HandleUnknown(std::exception_ptr ptr) noexcept
{
    try
    {
        if (ptr)
        {
            std::rethrow_exception(ptr);
        }

    }
    catch(const SysMonException& exception)
    {
        Handle(exception);
    }
    catch(const std::exception& e)
    {
        _ErrorDispatcher->Dispatch(BuildMinimalRecord("MinimumException", e.what(), typeid(e).name()));
    }
    catch(...)
    {
        _ErrorDispatcher->Dispatch(BuildMinimalRecord("UnknownException", "", ""));
    }
}

std::shared_ptr<IErrorDispatcher> ExceptionHandler::Dispatcher() const noexcept
{
    return _ErrorDispatcher;
}

ErrorRecord ExceptionHandler::BuildMinimalRecord(
    std::string category, 
    std::string message, 
    std::string type
) const noexcept
{
        LARGE_INTEGER ft;
        GetSystemTimeAsFileTime((FILETIME*)&ft);

        ErrorRecord record;

        record.UptimeMs     = GetTickCount64();
        record.Uuid         = std::to_string(record.UptimeMs);
        record.Severity     = ErrorSeverity::Error;
        record.Category     = category;
        record.UserId       = SysMonException::CaptureUserId();
        record.MemoryUsedKb = SysMonException::CaptureMemoryUsedKb();
        // context
        record.Context.ModuleName      = "";
        record.Context.Message         = message;
        record.Context.Type            = type;
        record.Context.Action          = "";
        record.Context.File            = "";
        record.Context.Function        = "";
        record.Context.Line            = 0;
        record.Context.Pid             = GetCurrentProcessId();
        record.Context.ThreadId        = GetCurrentThreadId();
        record.Context.FaultingAddress = 0;
        record.Context.Stacktrace      = SysMonException::CaptureStackTrace();

        record.Fingerprint = ErrorRecord::ComputeFingerprint(record);

        return record;
}