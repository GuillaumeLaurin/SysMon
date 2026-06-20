#include "reporter/ErrorDispatcher.hpp"

#include "reporter/ErrorQueue.hpp"

#include <algorithm>

ErrorDispatcher::ErrorDispatcher(std::shared_ptr<IErrorQueue> queue)
    : _Queue(queue),
      _Running(true),
      _Worker(std::thread(&ErrorDispatcher::WorkerLoop, this))
{
}

ErrorDispatcher::~ErrorDispatcher()
{
    Shutdown();
}

void ErrorDispatcher::Dispatch(ErrorRecord record) noexcept
{
    _Queue->Enqueue(std::move(record));
}

void ErrorDispatcher::AddSink(std::shared_ptr<IErrorSink> sink)
{
    auto lock = std::lock_guard(_Mutex);
    _Sinks[std::string(sink->Name())] = sink;
}

void ErrorDispatcher::RemoveSink(std::string_view name)
{
    auto lock = std::lock_guard(_Mutex);
    _Sinks.erase(std::string(name));
}

void ErrorDispatcher::AddFilter(std::shared_ptr<IErrorFilter> filter)
{
    auto lock = std::lock_guard(_Mutex);
    _Filters.push_back(filter);
}

void ErrorDispatcher::SetFormatter(std::shared_ptr<IErrorFormatter> formatter)
{
    auto lock = std::lock_guard(_Mutex);
    _Formatter = formatter;
}

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
    