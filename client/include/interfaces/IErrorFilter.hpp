#pragma once

#include "reporter/ErrorRecord.hpp"

class IErrorFilter
{
public:
    virtual ~IErrorFilter() = default;

    virtual bool ShouldProcess(const ErrorRecord& record) const noexcept = 0;
};