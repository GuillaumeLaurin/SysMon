#include "exceptions/ExceptionHandler.hpp"

#include "reporter/ErrorDispatcher.hpp"

#include "reporter/DumpProvider.hpp"

#include "reporter/ConsoleSink.hpp"

#include "reporter/JsonFormatter.hpp"

#include <typeinfo>

#include <filesystem>

ExceptionHandler::ExceptionHandler(
    std::shared_ptr<IErrorDispatcher> dispatcher,
    std::shared_ptr<IDumpProvider> dumpProvider
)
    : _ErrorDispatcher(dispatcher),
      _DumpProvider(dumpProvider),
      _OutputPath((std::filesystem::current_path() / "dumps").wstring()),
      _DumpType(DumpType::Full)
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
    auto record  = exception.ToRecord();
    bool isFatal = record.Severity == ErrorSeverity::Fatal;

    _ErrorDispatcher->Dispatch(record);

    if (isFatal)
    {
        _DumpProvider->GenerateDump(record, _OutputPath, _DumpType);
    }
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
        auto record = BuildMinimalRecord("MinimumException", e.what(), typeid(e).name());
        _ErrorDispatcher->Dispatch(record);
        _DumpProvider->GenerateDump(record, _OutputPath, _DumpType);
    }
    catch(...)
    {
        auto record = BuildMinimalRecord("UnknownException", "", "");
        _ErrorDispatcher->Dispatch(record);
        _DumpProvider->GenerateDump(record, _OutputPath, _DumpType);
    }
}

void ExceptionHandler::SetOutputPath(const std::wstring& outputPath) noexcept
{
    _OutputPath = outputPath;
}

void ExceptionHandler::SetDumpType(DumpType dumpType) noexcept
{
    _DumpType = dumpType;
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