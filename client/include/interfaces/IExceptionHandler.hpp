#pragma once

#include "exceptions/SysMonException.hpp"

#include "interfaces/IErrorDispatcher.hpp"

#include <memory>

class IExceptionHandler
{
public:
    virtual ~IExceptionHandler() = default;

    virtual void Handle(const SysMonException& exception) noexcept = 0;
    virtual void HandleUnknown(std::exception_ptr ptr) noexcept = 0;
    virtual std::shared_ptr<IErrorDispatcher> Dispatcher() const noexcept  = 0;
};