/**
 * @file IErrorFilter.hpp
 * @brief Contract for predicates deciding whether an error record is processed.
 */

#pragma once

#include "reporter/ErrorRecord.hpp"

/**
 * @brief Decides whether an error record should go through the reporting pipeline.
 */
class IErrorFilter
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IErrorFilter() = default;

    /**
     * @brief Returns true if the record should be processed, false to drop it.
     * @param record Error record to evaluate.
     * @return true if the record should be processed, false to drop it.
     */
    virtual bool ShouldProcess(const ErrorRecord& record) const noexcept = 0;
};
