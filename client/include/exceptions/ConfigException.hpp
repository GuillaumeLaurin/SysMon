#pragma once

#include "exceptions/SysMonException.hpp"

/**
 * @file ConfigException.hpp
 * @brief Declares ConfigException and its THROW_CONFIG convenience macro.
 */

/**
 * @brief Exception raised for configuration errors (Warning severity by default).
 */
class ConfigException : public SysMonException
{
    /** @brief Inherits all constructors from SysMonException. */
    using SysMonException::SysMonException;
};

/**
 * @brief Throws a ConfigException with Warning severity and the current source location.
 * @param msg Human-readable error message.
 * @param cat Error category.
 * @param mod Module name.
 * @param act Action being performed.
 * @throws ConfigException Always thrown with Warning severity.
 */
#ifndef THROW_CONFIG
#define THROW_CONFIG(msg, cat, mod, act) \
    throw ConfigException(msg, cat, mod, act, ErrorSeverity::Warning, __FILE__, __FUNCTION__, __LINE__)
#endif // !THROW_CONFIG
