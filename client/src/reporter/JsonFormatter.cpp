#include "reporter/JsonFormatter.hpp"

#include <nlohmann/json.hpp>

#include <sstream>
#include <ostream>
#include <iomanip> 

std::string JsonFormatter::Format(const ErrorRecord& record) const
{
    nlohmann::json object = {
        { "id",             record.Uuid },
        { "fingerprint",    record.Fingerprint },
        { "severity",       SeverityToString(record.Severity) },
        { "category",       record.Category },
        { "user_id",        record.UserId },
        { "timestamp",      TimestampToIso8601(record.Timestamp) },
        { "uptime_ms",      record.UptimeMs },
        { "memory_kb",      record.MemoryUsedKb },
        { "context", {
            { "message",            record.Context.Message },
            { "type",               record.Context.Type },
            { "action",             record.Context.Action },
            { "file",               record.Context.File },
            { "function",           record.Context.Function },
            { "line",               record.Context.Line },
            { "stacktrace",         record.Context.Stacktrace },
            { "module",             record.Context.ModuleName },
            { "pid",                record.Context.Pid },
            { "thread_id",          record.Context.ThreadId },
            { "faulting_address",   AddressToHexString(record.Context.FaultingAddress) }
        }},
        { "application", {
            { "name",               ErrorRecord::AppInfo.Name },
            { "version",            ErrorRecord::AppInfo.Version },
            { "build",              ErrorRecord::AppInfo.Build },
            { "git_commit",         ErrorRecord::AppInfo.GitCommit }
        }},
        { "system", {
            { "hostname",           ErrorRecord::SysInfo.HostName },
            { "ip",                 ErrorRecord::SysInfo.Ip },
            { "os",                 ErrorRecord::SysInfo.Os },
            { "arch",               ErrorRecord::SysInfo.Arch },
            { "username",           ErrorRecord::SysInfo.Username }
        }},
        { "metadata",       MetadataToJsonArray(record.Metadata) }
    };

    return object.dump(4);
}

std::string_view JsonFormatter::ContentType() const noexcept
{

    return "application/json";
}

std::string JsonFormatter::SeverityToString(ErrorSeverity severity)
{
    switch (severity)
    {
        case ErrorSeverity::Debug:
        {
            return "Debug";
        }
        case ErrorSeverity::Info:
        {
            return "Info";
        }
        case ErrorSeverity::Warning:
        {
            return "Warning";
        }
        case ErrorSeverity::Error:
        {
            return "Error";
        }
        case ErrorSeverity::Fatal:
        {
            return "Fatal";
        }
        default:
        {
            return "Unknown";
        }
    };
}

std::string JsonFormatter::TimestampToIso8601(LARGE_INTEGER timestamp)
{
    FILETIME local;

    FileTimeToLocalFileTime((FILETIME*)&timestamp, &local);
    SYSTEMTIME st;
    FileTimeToSystemTime(&local, &st);

    std::ostringstream oss;

    oss << std::setw(4) << std::setfill('0') << st.wYear << "-"
        << std::setw(2) << std::setfill('0') << st.wMonth << "-"
        << std::setw(2) << std::setfill('0') << st.wDay << "T"
        << std::setw(2) << std::setfill('0') << st.wHour << ":"
        << std::setw(2) << std::setfill('0') << st.wMinute << ":"
        << std::setw(2) << std::setfill('0') << st.wSecond << "."
        << std::setw(3) << std::setfill('0') << st.wMilliseconds << "Z";
    
    return oss.str();
}

std::string JsonFormatter::AddressToHexString(ULONG_PTR address)
{
    std::ostringstream oss;

    oss << "0x" << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << address;

    return oss.str();
}

nlohmann::json JsonFormatter::MetadataToJsonArray(const SysMon::Params& params)
{
    auto array = nlohmann::json::array();

    for (auto& param : params)
    {
        array.push_back(nlohmann::json({
            {"key",   param.first},
            {"value", param.second}
        }));
    }

    return array;
}