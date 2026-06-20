#pragma once

#include "interfaces/IErrorDispatcher.hpp"

#include "interfaces/IErrorQueue.hpp"

#include <mutex>

#include <vector>
#include <string>
#include <thread>
#include <atomic>

#include <unordered_map>

class ErrorDispatcher : public IErrorDispatcher
{
    using SinkMap = std::unordered_map<std::string, std::shared_ptr<IErrorSink>>;
    using FilterArray = std::vector<std::shared_ptr<IErrorFilter>>;
public:
    ErrorDispatcher(std::shared_ptr<IErrorQueue> queue); 

    ~ErrorDispatcher() override;

    void Dispatch(ErrorRecord record) noexcept override;

    void AddSink(std::shared_ptr<IErrorSink> sink) override;
    void RemoveSink(std::string_view name) override;

    void AddFilter(std::shared_ptr<IErrorFilter> filter) override;

    void SetFormatter(std::shared_ptr<IErrorFormatter> formatter) override;

    void Flush() noexcept override;
    void Shutdown() noexcept override;

private:
    std::shared_ptr<IErrorQueue>               _Queue;
    SinkMap                                    _Sinks;
    FilterArray                                _Filters;
    std::shared_ptr<IErrorFormatter>           _Formatter;
    std::thread                                _Worker;
    std::atomic<bool>                          _Running;
    std::mutex                                 _Mutex;

    void WorkerLoop();
};