#pragma once

#include "reporter/ErrorRecord.hpp"

#include <string>

#include <stdexcept>

class SysMonException : public std::runtime_error
{
public:
    SysMonException(
        const std::string& message, 
        const std::string& category, 
        const std::string& moduleName,
        const std::string& action,
        ErrorSeverity      severity,
        const char*        file,
        const char*        function,
        ULONG              line
    );

    virtual std::string_view Category() const noexcept;
    virtual ErrorSeverity    Severity() const noexcept;
    virtual ErrorRecord      ToRecord() const noexcept;

    static std::string CaptureStackTrace() noexcept;
    static std::string CaptureUserId() noexcept;
    static ULONG CaptureMemoryUsedKb() noexcept;

protected:
    std::string const   _Category;
    std::string const   _ModuleName;
    std::string const   _Action;
    ErrorSeverity const _Severity;
    const char*         _File;
    const char*         _Function;
    ULONG               _Line;

    inline static bool  _IsInitialized = false;

    static void SymbolInitialize() noexcept;
};

#ifndef THROW_SYSMON
#define THROW_SYSMON(msg, cat, mod, act) \
    throw SysMonException(msg, cat, mod, act, ErrorSeverity::Error, __FILE__, __FUNCTION__, __LINE__)
#endif // !THROW_SYSMON