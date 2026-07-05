/**
 * @file ErrorDispatcher.hpp
 * @brief Worker-thread based IErrorDispatcher implementation tying together
 *        the queue, filters, formatter and sinks of the reporter pipeline.
 */

#pragma once

#include "interfaces/IErrorDispatcher.hpp"

#include "interfaces/IErrorQueue.hpp"

#include <mutex>

#include <vector>
#include <string>
#include <thread>
#include <atomic>

#include <unordered_map>

/**
 * @brief IErrorDispatcher implementation: a worker thread drains the queue,
 *        applies the filters, formats each record and fans it out to the sinks.
 * @note Thread-safe: sinks, filters and the formatter are protected by an
 *       internal mutex; Dispatch() only enqueues and never blocks the caller.
 */
class ErrorDispatcher : public IErrorDispatcher
{
    /** @brief Sinks registered on the dispatcher, keyed by their Name(). */
    using SinkMap = std::unordered_map<std::string, std::shared_ptr<IErrorSink>>;
    /** @brief Ordered collection of filters applied to every record. */
    using FilterArray = std::vector<std::shared_ptr<IErrorFilter>>;
public:
    /**
     * @brief Constructs the dispatcher and immediately starts the worker thread.
     * @param queue Queue the worker thread drains records from.
     */
    ErrorDispatcher(std::shared_ptr<IErrorQueue> queue);

    /** @brief Shuts the pipeline down if still running (joins the worker thread). */
    ~ErrorDispatcher() override;

    /**
     * @brief Enqueues a record for asynchronous processing.
     * @param record Record to dispatch.
     */
    void Dispatch(ErrorRecord record) noexcept override;

    /**
     * @brief Adds an output sink, keyed by its Name().
     * @param sink Sink to register; replaces any existing sink with the same name.
     */
    void AddSink(std::shared_ptr<IErrorSink> sink) override;

    /**
     * @brief Removes the sink registered under @p name.
     * @param name Name of the sink to remove.
     */
    void RemoveSink(std::string_view name) override;

    /**
     * @brief Adds a filter; a record is dropped unless every filter accepts it.
     * @param filter Filter to append to the filter chain.
     */
    void AddFilter(std::shared_ptr<IErrorFilter> filter) override;

    /**
     * @brief Sets the formatter used to serialize records before sending.
     * @param formatter Formatter instance; records are dropped while none is set.
     */
    void SetFormatter(std::shared_ptr<IErrorFormatter> formatter) override;

    /**
     * @brief Blocks until the queue is empty, then flushes every sink.
     * @note Does not stop the worker thread; safe to call while running.
     */
    void Flush() noexcept override;

    /**
     * @brief Stops the worker thread and shuts the sinks down.
     * @note Idempotent: safe to call multiple times (e.g. from the destructor).
     */
    void Shutdown() noexcept override;

private:
    std::shared_ptr<IErrorQueue>               _Queue;     ///< Source queue drained by the worker thread.
    SinkMap                                    _Sinks;     ///< Registered output sinks, guarded by _Mutex.
    FilterArray                                _Filters;   ///< Filter chain, guarded by _Mutex.
    std::shared_ptr<IErrorFormatter>           _Formatter; ///< Active formatter, guarded by _Mutex.
    std::thread                                _Worker;    ///< Background thread running WorkerLoop().
    std::atomic<bool>                          _Running;   ///< Controls the worker thread's main loop.
    std::mutex                                 _Mutex;     ///< Guards _Sinks, _Filters and _Formatter.

    /**
     * @brief Worker thread body: dequeue, filter, format, send.
     * @note Runs on a dedicated background thread until Shutdown() clears _Running
     *       and the queue is shut down.
     */
    void WorkerLoop();
};
