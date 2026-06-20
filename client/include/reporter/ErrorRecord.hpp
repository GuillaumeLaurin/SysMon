#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <string>

#include <vector>

#include <utility>

enum class ErrorSeverity
{
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

namespace SysMon
{
    using Params =  std::vector<std::pair<std::string, std::string>>;

    struct ApplicationInformation
    {
        std::string Name;
        std::string Version;
        std::string Build;
        std::string GitCommit;
    };

    struct SystemInformation
    {
        std::string HostName;
        std::string Ip;
        std::string Os;
        std::string Arch;
        std::string Username;
    };

    struct ErrorContext
    {
        std::string Stacktrace;
        std::string ModuleName;
        std::string Message;
        std::string Type;
        std::string Action;
        const char* File;
        const char* Function;
        ULONG       Line;
        ULONG       Pid;
        ULONG       ThreadId;
        ULONG_PTR   FaultingAddress;
    };
} // namespace SysMon

struct ErrorRecord
{
    static SysMon::ApplicationInformation AppInfo;
    static SysMon::SystemInformation      SysInfo;
    SysMon::ErrorContext                  Context;
    std::string                           Uuid;
    std::string                           UserId;
    std::string                           Fingerprint;
    std::string                           Category;
    ULONGLONG                             UptimeMs;
    LARGE_INTEGER                         Timestamp;
    SysMon::Params                        Metadata;
    ULONG                                 MemoryUsedKb;
    ErrorSeverity                         Severity;

    static std::string ComputeFingerprint(const ErrorRecord& report);
private:
    static uint64_t Fnv1a(std::string_view data, uint64_t hash);
};