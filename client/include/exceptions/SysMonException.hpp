#pragma once

#include "reporter/ErrorRecord.hpp"

#include <string>

#include <stdexcept>

/**
 * @file SysMonException.hpp
 * @brief Declares SysMonException, the base exception type carrying the
 *        reporting metadata required to build ErrorRecord instances, and
 *        the THROW_SYSMON convenience macro.
 */

/**
 * @brief Base exception of the client. Carries the reporting metadata
 *        (category, module, action, severity, source location) needed to
 *        build a full ErrorRecord. Prefer the THROW_* macros to raw throws.
 */
class SysMonException : public std::runtime_error
{
public:
    /**
     * @brief Constructs the exception with its full reporting metadata.
     * @param message Human-readable error message (passed to std::runtime_error).
     * @param category Error category (e.g. "Storage", "Device").
     * @param moduleName Name of the module that raised the exception.
     * @param action Action being performed when the exception was raised.
     * @param severity Severity carried by the exception.
     * @param file Source file where the exception was thrown (typically __FILE__).
     * @param function Function where the exception was thrown (typically __FUNCTION__).
     * @param line Source line where the exception was thrown (typically __LINE__).
     */
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

    /**
     * @brief Error category (e.g. "Storage", "Device").
     * @return The category string associated with the exception.
     */
    virtual std::string_view Category() const noexcept;

    /**
     * @brief Severity carried by the exception.
     * @return The severity associated with the exception.
     */
    virtual ErrorSeverity    Severity() const noexcept;

    /**
     * @brief Builds a complete ErrorRecord (stack trace, user, memory usage included).
     * @return A fully populated ErrorRecord describing this exception.
     */
    virtual ErrorRecord      ToRecord() const noexcept;

    /**
     * @brief Captures the current call stack as readable text (DbgHelp).
     * @return A multi-line string describing each stack frame.
     */
    static std::string CaptureStackTrace() noexcept;

    /**
     * @brief Returns the current Windows user name.
     * @return The current user name, or "unknown" if it could not be retrieved.
     */
    static std::string CaptureUserId() noexcept;

    /**
     * @brief Returns the working-set size of the process, in KiB.
     * @return The process working-set size in kilobytes.
     */
    static ULONG CaptureMemoryUsedKb() noexcept;

protected:
    /** @brief Error category (e.g. "Storage", "Device"). */
    std::string const   _Category;
    /** @brief Name of the module that raised the exception. */
    std::string const   _ModuleName;
    /** @brief Action being performed when the exception was raised. */
    std::string const   _Action;
    /** @brief Severity carried by the exception. */
    ErrorSeverity const _Severity;
    /** @brief Source file where the exception was thrown. */
    const char*         _File;
    /** @brief Function where the exception was thrown. */
    const char*         _Function;
    /** @brief Source line where the exception was thrown. */
    ULONG               _Line;

    /** @brief Tracks whether the DbgHelp symbol handler has been initialized. */
    inline static bool  _IsInitialized = false;

    /** @brief Lazily initializes the DbgHelp symbol handler (once per process). */
    static void SymbolInitialize() noexcept;
};

/**
 * @brief Throws a SysMonException with Error severity and the current source location.
 * @param msg Human-readable error message.
 * @param cat Error category.
 * @param mod Module name.
 * @param act Action being performed.
 * @throws SysMonException Always thrown with Error severity.
 */
#ifndef THROW_SYSMON
#define THROW_SYSMON(msg, cat, mod, act) \
    throw SysMonException(msg, cat, mod, act, ErrorSeverity::Error, __FILE__, __FUNCTION__, __LINE__)
#endif // !THROW_SYSMON
