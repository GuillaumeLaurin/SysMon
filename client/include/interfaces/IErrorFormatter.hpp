#pragma once

#include "reporter/ErrorRecord.hpp"

class IErrorFormatter
{
public:
    virtual ~IErrorFormatter() = default;

    virtual std::string Format(const ErrorRecord& record) const = 0;

    virtual std::string_view ContentType() const noexcept = 0;
};