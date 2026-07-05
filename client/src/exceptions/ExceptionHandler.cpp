#include "exceptions/ExceptionHandler.hpp"

#include "reporter/ErrorDispatcher.hpp"

#include "reporter/DumpProvider.hpp"

#include <typeinfo>

#include <filesystem>

/**
 * @file ExceptionHandler.cpp
 * @brief Implements ExceptionHandler: exception-to-ErrorRecord conversion,
 *        dispatch and fatal-error dump generation.
 */

/** @brief Constructs the exception handler with its dependencies. */
ExceptionHandler::ExceptionHandler(
    std::shared_ptr<IErrorDispatcher> dispatcher,
    std::shared_ptr<IDumpProvider> dumpProvider
)
    : _ErrorDispatcher(dispatcher),
      _DumpProvider(dumpProvider),
      _OutputPath((std::filesystem::current_path() / "dumps").wstring()),
      _DumpType(DumpType::Full)
{
}

/** @brief Reports a typed SysMon exception; fatal ones also produce a dump. */
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

/** @brief Reports an unknown/foreign exception captured via std::current_exception(). */
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

/** @brief Sets the directory/file path used for generated dumps. */
void ExceptionHandler::SetOutputPath(const std::wstring& outputPath) noexcept
{
    _OutputPath = outputPath;
}

/** @brief Sets the kind of dump produced on fatal errors. */
void ExceptionHandler::SetDumpType(DumpType dumpType) noexcept
{
    _DumpType = dumpType;
}

/** @brief Dispatcher used to emit the resulting error records. */
std::shared_ptr<IErrorDispatcher> ExceptionHandler::Dispatcher() const noexcept
{
    return _ErrorDispatcher;
}

/** @brief Builds an ErrorRecord for exceptions that carry no SysMon metadata. */
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
        record.Timestamp    = ft;
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