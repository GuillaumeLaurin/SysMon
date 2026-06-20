#pragma once

#include "interfaces/IErrorFilter.hpp"

class SeverityFilter : public IErrorFilter
{
public:
    SeverityFilter(ErrorSeverity minimumSeverity);

    ~SeverityFilter() override = default;

    bool ShouldProcess(const ErrorRecord& record) const noexcept override;

private:
    ErrorSeverity _MinimumSeverity;
};