#pragma once

#include "exceptions/SysMonException.hpp"

/**
 * @file DeviceException.hpp
 * @brief Declares DeviceException and its THROW_DEVICE convenience macro.
 */

/**
 * @brief Exception raised for driver/device communication errors (Fatal severity by default).
 */
class DeviceException : public SysMonException
{
    /** @brief Inherits all constructors from SysMonException. */
    using SysMonException::SysMonException;
};

/**
 * @brief Throws a DeviceException with Fatal severity and the current source location.
 * @param msg Human-readable error message.
 * @param cat Error category.
 * @param mod Module name.
 * @param act Action being performed.
 * @throws DeviceException Always thrown with Fatal severity.
 */
#ifndef THROW_DEVICE
#define THROW_DEVICE(msg, cat, mod, act) \
    throw DeviceException(msg, cat, mod, act, ErrorSeverity::Fatal, __FILE__, __FUNCTION__, __LINE__)
#endif // !THROW_DEVICE
