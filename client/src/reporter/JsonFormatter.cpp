/**
 * @file JsonFormatter.cpp
 * @brief Implementation of JsonFormatter.
 */

#include "reporter/JsonFormatter.hpp"

#include <nlohmann/json.hpp>

#include <sstream>
#include <ostream>
#include <iomanip>

/** @brief Builds a nlohmann::json object covering record, app info and system info, and dumps it with 4-space indentation. */
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

/** @brief Always returns "application/json". */
std::string_view JsonFormatter::ContentType() const noexcept
{

    return "application/json";
}

/** @brief Switches over ErrorSeverity, returning "Unknown" for unrecognized values. */
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

/**
 * @brief Reinterprets the LARGE_INTEGER as a FILETIME and formats it as
 *        ISO-8601 with a "Z" (UTC) suffix, without any local-time conversion.
 */
std::string JsonFormatter::TimestampToIso8601(LARGE_INTEGER timestamp)
{
    FILETIME ft;
    ft.dwLowDateTime  = timestamp.LowPart;
    ft.dwHighDateTime = static_cast<DWORD>(timestamp.HighPart);

    // The suffix "Z" means UTC: format the FILETIME (already UTC) directly,
    // without converting to local time.
    SYSTEMTIME st{};

    if (!FileTimeToSystemTime(&ft, &st))
        return "1970-01-01T00:00:00.000Z";

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

/** @brief Formats the address as a zero-padded, uppercase 0x-prefixed hex string. */
std::string JsonFormatter::AddressToHexString(ULONG_PTR address)
{
    std::ostringstream oss;

    oss << "0x" << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << address;

    return oss.str();
}

/** @brief Maps each metadata pair to a {"key", "value"} JSON object. */
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