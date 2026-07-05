/**
 * @file IErrorDispatcher.hpp
 * @brief Contract for the central error-reporting pipeline.
 */

#pragma once

#include "reporter/ErrorRecord.hpp"

#include "interfaces/IErrorSink.hpp"

#include "interfaces/IErrorFormatter.hpp"

#include "interfaces/IErrorFilter.hpp"

#include <memory>

/**
 * @brief Central error-reporting pipeline: filters incoming records,
 *        formats them and fans them out to the registered sinks.
 *
 * @note Implementations are expected to be thread-safe: Dispatch() is typically
 *       called from multiple worker threads while AddSink()/AddFilter()/SetFormatter()
 *       may be called during setup.
 */
class IErrorDispatcher
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     * @note Implementations should call Shutdown() if not already done.
     */
    virtual ~IErrorDispatcher() = default;

    /**
     * @brief Submits an error record to the pipeline (asynchronous).
     * @param record Error record to process; taken by value and moved into the pipeline.
     */
    virtual void Dispatch(ErrorRecord record) noexcept = 0;

    /**
     * @brief Adds an output sink (console, file, ...).
     * @param sink Shared ownership of the sink to register.
     */
    virtual void AddSink(std::shared_ptr<IErrorSink> sink) = 0;

    /**
     * @brief Removes the sink registered under @p name.
     * @param name Name of the sink to remove, as returned by IErrorSink::Name().
     */
    virtual void RemoveSink(std::string_view name) = 0;

    /**
     * @brief Adds a filter; a record is dropped unless every filter accepts it.
     * @param filter Shared ownership of the filter to register.
     */
    virtual void AddFilter(std::shared_ptr<IErrorFilter> filter) = 0;

    /**
     * @brief Sets the formatter used to serialize records before sending.
     * @param formatter Shared ownership of the formatter to use.
     */
    virtual void SetFormatter(std::shared_ptr<IErrorFormatter> formatter) = 0;

    /**
     * @brief Blocks until the pending queue has been drained.
     */
    virtual void Flush() noexcept = 0;

    /**
     * @brief Stops the worker and flushes the sinks.
     * @note No further Dispatch() call is expected after Shutdown() returns.
     */
    virtual void Shutdown() noexcept = 0;
};
