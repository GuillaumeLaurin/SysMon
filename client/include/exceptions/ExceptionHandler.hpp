#pragma once

#include "interfaces/IExceptionHandler.hpp"

#include "interfaces/IErrorDispatcher.hpp"

#include "interfaces/IDumpProvider.hpp"

#include <memory>

class ExceptionHandler : public IExceptionHandler
{
public:
    ExceptionHandler(
        std::shared_ptr<IErrorDispatcher> dispatcher = nullptr,
        std::shared_ptr<IDumpProvider> dumpProvider = nullptr
    );

    ~ExceptionHandler() override = default;

    void Handle(const SysMonException& exception) noexcept override;
    void HandleUnknown(std::exception_ptr ptr) noexcept override;

    void SetOutputPath(const std::wstring& outputPath) noexcept;
    void SetDumpType(DumpType dumpType) noexcept;
    
    std::shared_ptr<IErrorDispatcher> Dispatcher() const noexcept override;

private:
    std::shared_ptr<IErrorDispatcher> _ErrorDispatcher;
    std::shared_ptr<IDumpProvider>    _DumpProvider;
    std::wstring                      _OutputPath;
    DumpType                          _DumpType;

    ErrorRecord BuildMinimalRecord(
        std::string category, 
        std::string message, 
        std::string type
    ) const noexcept;
};