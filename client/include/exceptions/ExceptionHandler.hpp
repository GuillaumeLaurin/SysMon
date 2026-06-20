#pragma once

#include "interfaces/IExceptionHandler.hpp"

#include "interfaces/IErrorDispatcher.hpp"

#include <memory>

class ExceptionHandler : public IExceptionHandler
{
public:
    ExceptionHandler(std::shared_ptr<IErrorDispatcher> dispatcher = nullptr);

    ~ExceptionHandler() override = default;

    void Handle(const SysMonException& exception) noexcept override;
    void HandleUnknown(std::exception_ptr ptr) noexcept override;
    
    std::shared_ptr<IErrorDispatcher> Dispatcher() const noexcept override;

private:
    std::shared_ptr<IErrorDispatcher> _ErrorDispatcher;

    ErrorRecord BuildMinimalRecord(
        std::string category, 
        std::string message, 
        std::string type
    ) const noexcept;
};