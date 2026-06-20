#pragma once

#include "exceptions/SysMonException.hpp"

class DeviceException : public SysMonException
{
    using SysMonException::SysMonException;
};

#ifndef THROW_DEVICE
#define THROW_DEVICE(msg, cat, mod, act) \
    throw DeviceException(msg, cat, mod, act, ErrorSeverity::Fatal, __FILE__, __FUNCTION__, __LINE__)
#endif // !THROW_DEVICE