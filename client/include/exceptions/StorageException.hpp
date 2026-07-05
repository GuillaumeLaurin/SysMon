#pragma once

#include "exceptions/SysMonException.hpp"

/**
 * @file StorageException.hpp
 * @brief Declares StorageException and its THROW_STORAGE convenience macro.
 */

/**
 * @brief Exception raised for database/storage errors (Fatal severity by default).
 */
class StorageException : public SysMonException
{
    /** @brief Inherits all constructors from SysMonException. */
    using SysMonException::SysMonException;
};

/**
 * @brief Throws a StorageException with Fatal severity and the current source location.
 * @param msg Human-readable error message.
 * @param cat Error category.
 * @param mod Module name.
 * @param act Action being performed.
 * @throws StorageException Always thrown with Fatal severity.
 */
#ifndef THROW_STORAGE
#define THROW_STORAGE(msg, cat, mod, act) \
    throw StorageException(msg, cat, mod, act, ErrorSeverity::Fatal, __FILE__, __FUNCTION__, __LINE__)
#endif // !THROW_STORAGE
