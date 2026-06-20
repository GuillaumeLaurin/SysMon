#include "exceptions/SysMonException.hpp"

#include <vector>

#include <sstream>
#include <ostream>

#include <typeinfo>

#include <psapi.h>

#include <dbghelp.h>

SysMonException::SysMonException(
    const std::string& message, 
    const std::string& category, 
    const std::string& moduleName,
    const std::string& action,
    ErrorSeverity      severity,
    const char*        file,
    const char*        function,
    ULONG              line
)
    : std::runtime_error(message), _Category(category), _ModuleName(moduleName),
      _Action(action), _Severity(severity), _File(file), _Function(function), 
      _Line(line)
{
}

std::string_view SysMonException::Category() const noexcept
{
    return _Category;
}

ErrorSeverity SysMonException::Severity() const noexcept
{
    return _Severity;
}

ErrorRecord SysMonException::ToRecord() const noexcept
{
    LARGE_INTEGER ft;
    GetSystemTimeAsFileTime((FILETIME*)&ft);

    ErrorRecord record;

    record.UptimeMs     = GetTickCount64();
    record.Uuid         = std::to_string(record.UptimeMs);
    record.Timestamp    = ft;
    record.Severity     = _Severity;
    record.Category     = _Category;
    record.UserId       = CaptureUserId();
    record.MemoryUsedKb = CaptureMemoryUsedKb();
    // context
    record.Context.ModuleName      = _ModuleName;
    record.Context.Message         = what();
    record.Context.Type            = typeid(*this).name();
    record.Context.Action          = _Action;
    record.Context.File            = _File;
    record.Context.Function        = _Function;
    record.Context.Line            = _Line;
    record.Context.Pid             = GetCurrentProcessId();
    record.Context.ThreadId        = GetCurrentThreadId();
    record.Context.FaultingAddress = 0;
    record.Context.Stacktrace      = CaptureStackTrace();

    record.Fingerprint = ErrorRecord::ComputeFingerprint(record);

    return record;
}

std::string SysMonException::CaptureStackTrace() noexcept
{
    HANDLE process = GetCurrentProcess();

    SymbolInitialize();

    void* frames[64];
    auto count = CaptureStackBackTrace(0, 64, frames, nullptr);

    std::vector<char> buffer(sizeof(SYMBOL_INFO) + MAX_SYM_NAME);
    SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(buffer.data());
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    std::ostringstream oss;

    for (int i = 0; i < count; i++)
    {
        auto addr = (DWORD64)frames[i];
        SymFromAddr(process, addr, nullptr, symbol);

        oss << i << ": " << symbol->Name;

        IMAGEHLP_LINE64 line = {};
        DWORD displacement = 0;
        if (SymGetLineFromAddr64(process, addr, &displacement, &line))
        {
            oss << " (" << line.FileName << ":" << line.LineNumber << ")";
        }
        oss << "\n";
    }

    return oss.str();
}

std::string SysMonException::CaptureUserId() noexcept
{
    char username[256];
    DWORD size = 256;

    if(!GetUserNameA(username, &size))
    {
        return "unknown";
    }
    
    return std::string(username);
}

ULONG SysMonException::CaptureMemoryUsedKb() noexcept
{
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return static_cast<ULONG>(pmc.WorkingSetSize / 1024);
}

void SysMonException::SymbolInitialize() noexcept
{
    if (!_IsInitialized)
    {
        _IsInitialized = true;
        SymInitialize(GetCurrentProcess(), nullptr, TRUE);
    }
}