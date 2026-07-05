/**
 * @file ErrorDispatcher.cpp
 * @brief Implementation of ErrorDispatcher: worker thread lifecycle and
 *        registration of sinks/filters/formatter.
 */

#include "reporter/ErrorDispatcher.hpp"

#include "reporter/ErrorQueue.hpp"

#include <algorithm>

/** @brief Stores the queue and starts the worker thread running WorkerLoop(). */
ErrorDispatcher::ErrorDispatcher(std::shared_ptr<IErrorQueue> queue)
    : _Queue(queue),
      _Running(true),
      _Worker(std::thread(&ErrorDispatcher::WorkerLoop, this))
{
}

/** @brief Calls Shutdown() to stop the worker thread and flush the sinks. */
ErrorDispatcher::~ErrorDispatcher()
{
    Shutdown();
}

/** @brief Forwards the record to the underlying queue. */
void ErrorDispatcher::Dispatch(ErrorRecord record) noexcept
{
    _Queue->Enqueue(std::move(record));
}

/** @brief Registers/replaces a sink in the sink map under lock. */
void ErrorDispatcher::AddSink(std::shared_ptr<IErrorSink> sink)
{
    auto lock = std::lock_guard(_Mutex);
    _Sinks[std::string(sink->Name())] = sink;
}

/** @brief Erases the named sink from the sink map under lock. */
void ErrorDispatcher::RemoveSink(std::string_view name)
{
    auto lock = std::lock_guard(_Mutex);
    _Sinks.erase(std::string(name));
}

/** @brief Appends the filter to the filter chain under lock. */
void ErrorDispatcher::AddFilter(std::shared_ptr<IErrorFilter> filter)
{
    auto lock = std::lock_guard(_Mutex);
    _Filters.push_back(filter);
}

/** @brief Replaces the active formatter under lock. */
void ErrorDispatcher::SetFormatter(std::shared_ptr<IErrorFormatter> formatter)
{
    auto lock = std::lock_guard(_Mutex);
    _Formatter = formatter;
}

/** @brief Takes a snapshot of the sinks under lock, then flushes each outside the lock. */
void ErrorDispatcher::Flush() noexcept
{
    SinkMap localSinks;

    {
        auto lock = std::lock_guard(_Mutex);
        localSinks = _Sinks;
    }

    for (auto& sink : localSinks)
    {
        sink.second->Flush();
    }
}

/** @brief Clears the running flag, shuts the queue down, joins the worker thread, then flushes. */
void ErrorDispatcher::Shutdown() noexcept
{
    _Running = false;

    _Queue->Shutdown();

    if (_Worker.joinable())
    {
        _Worker.join();
    }

    Flush();
}

/**
 * @brief Repeatedly dequeues (100ms timeout), snapshots sinks/filters/formatter
 *        under lock, drops the record if no formatter is set or any filter
 *        rejects it, then formats and sends it to every sink.
 * @note Runs on the background worker thread for the lifetime of the dispatcher.
 */
void ErrorDispatcher::WorkerLoop()
{
    while (_Running)
    {
        ErrorRecord record;
        bool status = _Queue->Dequeue(record, 100);

        if (!status)
        {
            continue;
        }

        SinkMap localSinks;
        FilterArray localFilters;
        std::shared_ptr<IErrorFormatter> localFormatter;

        {
            auto lock = std::lock_guard(_Mutex);
            localSinks     = _Sinks;
            localFilters   = _Filters;
            localFormatter = _Formatter;
        }

        if (localFormatter == nullptr)
        {
            continue;
        }

        bool shouldProcess = std::all_of(localFilters.begin(), localFilters.end(), 
            [&](const auto& filter) { return filter->ShouldProcess(record); });

        if (!shouldProcess)
        {
            continue;
        }

        auto payload = localFormatter->Format(record);
        auto contentType = localFormatter->ContentType();

        for (auto& sink : localSinks)
        {
            sink.second->Send(payload, contentType);
        }
    }
}
    