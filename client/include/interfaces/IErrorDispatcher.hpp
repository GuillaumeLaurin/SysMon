#pragma once

#include "reporter/ErrorRecord.hpp"

#include "interfaces/IErrorSink.hpp"

#include "interfaces/IErrorFormatter.hpp"

#include "interfaces/IErrorFilter.hpp"

#include <memory>

class IErrorDispatcher
{
public:
    virtual ~IErrorDispatcher() = default;

    virtual void Dispatch(ErrorRecord record) noexcept = 0;

    virtual void AddSink(std::shared_ptr<IErrorSink> sink) = 0;
    virtual void RemoveSink(std::string_view name) = 0;

    virtual void AddFilter(std::shared_ptr<IErrorFilter> filter) = 0;

    virtual void SetFormatter(std::shared_ptr<IErrorFormatter> formatter) = 0;

    virtual void Flush() noexcept = 0;
    virtual void Shutdown() noexcept = 0;
};