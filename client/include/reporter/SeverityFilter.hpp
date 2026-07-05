/**
 * @file SeverityFilter.hpp
 * @brief IErrorFilter implementation that blocks error records below a
 *        configured minimum severity.
 */

#pragma once

#include "interfaces/IErrorFilter.hpp"

/**
 * @brief Filter dropping records below a minimum severity.
 */
class SeverityFilter : public IErrorFilter
{
public:
    /**
     * @brief Constructs the filter with the given severity floor.
     * @param minimumSeverity Lowest severity that is still processed.
     */
    SeverityFilter(ErrorSeverity minimumSeverity);

    /** @brief Default destructor. */
    ~SeverityFilter() override = default;

    /**
     * @brief Returns true when the record severity is at least the configured minimum.
     * @param record Record to evaluate.
     * @return true if record.Severity >= the configured minimum severity.
     */
    bool ShouldProcess(const ErrorRecord& record) const noexcept override;

private:
    ErrorSeverity _MinimumSeverity; ///< Lowest severity that passes the filter.
};
