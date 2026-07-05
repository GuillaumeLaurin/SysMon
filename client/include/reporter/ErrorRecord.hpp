/**
 * @file ErrorRecord.hpp
 * @brief Data structures describing a single error report flowing through
 *        the reporter pipeline (context, environment info and severity).
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <string>

#include <vector>

#include <utility>

/**
 * @brief Severity levels attached to error records and exceptions.
 *        Ordered from least to most severe so relational comparisons
 *        (e.g. in SeverityFilter) work as expected.
 */
enum class ErrorSeverity
{
    Debug,   ///< Diagnostic information, no action required.
    Info,    ///< Informational event.
    Warning, ///< Unexpected but recoverable condition.
    Error,   ///< Operation failed; application can continue.
    Fatal    ///< Unrecoverable condition; application is expected to terminate.
};

namespace SysMon
{
    /**
     * @brief Ordered key/value pairs attached to a record as free-form metadata.
     */
    using Params =  std::vector<std::pair<std::string, std::string>>;

    /**
     * @brief Static information about the running application.
     */
    struct ApplicationInformation
    {
        std::string Name;      ///< Application name
        std::string Version;   ///< Semantic version
        std::string Build;     ///< Build identifier
        std::string GitCommit; ///< Source commit hash
    };

    /**
     * @brief Static information about the host system.
     */
    struct SystemInformation
    {
        std::string HostName; ///< Machine name
        std::string Ip;       ///< Primary IP address
        std::string Os;       ///< Operating system name/version
        std::string Arch;     ///< CPU architecture
        std::string Username; ///< Session user name
    };

    /**
     * @brief Details about the error itself and where it occurred.
     */
    struct ErrorContext
    {
        std::string Stacktrace;      ///< Captured call stack (text)
        std::string ModuleName;      ///< Module reporting the error
        std::string Message;         ///< Human-readable message
        std::string Type;            ///< Exception/error type name
        std::string Action;          ///< Action being performed when it happened
        const char* File;            ///< Source file
        const char* Function;        ///< Source function
        ULONG       Line;            ///< Source line
        ULONG       Pid;             ///< Process ID
        ULONG       ThreadId;        ///< Thread ID
        ULONG_PTR   FaultingAddress; ///< Faulting address, when applicable
    };
} // namespace SysMon

/**
 * @brief Complete error report flowing through the reporting pipeline.
 *        AppInfo/SysInfo are process-wide and set once at startup.
 */
struct ErrorRecord
{
    static SysMon::ApplicationInformation AppInfo; ///< Shared application info
    static SysMon::SystemInformation      SysInfo; ///< Shared host info
    SysMon::ErrorContext                  Context;      ///< Error details
    std::string                           Uuid;         ///< Unique report identifier
    std::string                           UserId;       ///< Reporting user
    std::string                           Fingerprint;  ///< Deduplication fingerprint
    std::string                           Category;     ///< Error category
    ULONGLONG                             UptimeMs;     ///< Process uptime at capture time
    LARGE_INTEGER                         Timestamp;    ///< Capture time
    SysMon::Params                        Metadata;     ///< Free-form key/value pairs
    ULONG                                 MemoryUsedKb; ///< Working set at capture time
    ErrorSeverity                         Severity;     ///< Record severity

    /**
     * @brief Computes a stable fingerprint (type + module + location) for deduplication.
     * @param report Record whose context is hashed.
     * @return Hexadecimal FNV-1a hash string uniquely identifying the error site.
     * @note Used by filters (e.g. RateLimitFilter) and by DumpProvider to name dump files.
     */
    static std::string ComputeFingerprint(const ErrorRecord& report);
private:
    /**
     * @brief FNV-1a hash step used by ComputeFingerprint().
     * @param data Bytes to fold into the hash.
     * @param hash Running hash value to update.
     * @return Updated hash value after processing @p data.
     */
    static uint64_t Fnv1a(std::string_view data, uint64_t hash);
};
