#pragma once

#include "exceptions/SysMonException.hpp"

class ConfigException : public SysMonException
{
    using SysMonException::SysMonException;
};

#ifndef THROW_CONFIG
#define THROW_CONFIG(msg, cat, mod, act) \
    throw ConfigException(msg, cat, mod, act, ErrorSeverity::Warning, __FILE__, __FUNCTION__, __LINE__)
#endif // !THROW_CONFIG