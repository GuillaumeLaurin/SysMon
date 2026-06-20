#include "reporter/SeverityFilter.hpp"

SeverityFilter::SeverityFilter(ErrorSeverity minimumSeverity)
    : _MinimumSeverity(minimumSeverity)
{
}

bool SeverityFilter::ShouldProcess(const ErrorRecord& record) const noexcept
{
    auto severity = record.Severity;

    return severity >= _MinimumSeverity;
}