/**
 * @file SeverityFilter.cpp
 * @brief Implementation of SeverityFilter.
 */

#include "reporter/SeverityFilter.hpp"

/** @brief Stores the minimum severity threshold. */
SeverityFilter::SeverityFilter(ErrorSeverity minimumSeverity)
    : _MinimumSeverity(minimumSeverity)
{
}

/** @brief Compares the record's severity against the configured minimum. */
bool SeverityFilter::ShouldProcess(const ErrorRecord& record) const noexcept
{
    auto severity = record.Severity;

    return severity >= _MinimumSeverity;
}