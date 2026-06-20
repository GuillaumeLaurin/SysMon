#pragma once

#include "exceptions/SysMonException.hpp"

class StorageException : public SysMonException
{
    using SysMonException::SysMonException;
};

#ifndef THROW_STORAGE
#define THROW_STORAGE(msg, cat, mod, act) \
    throw StorageException(msg, cat, mod, act, ErrorSeverity::Fatal, __FILE__, __FUNCTION__, __LINE__)
#endif // !THROW_STORAGE