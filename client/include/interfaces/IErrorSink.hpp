#pragma once

#include <string>

class IErrorSink
{
public:
    virtual ~IErrorSink() = default;

    virtual void Send(std::string_view payload, std::string_view contentType) noexcept = 0;

    virtual void Flush() noexcept = 0;
    virtual void Shutdown() noexcept = 0;

    virtual std::string_view Name() const noexcept = 0;
};