#pragma once

#include "interfaces/IErrorQueue.hpp"

#include <queue>

#include <mutex>

#include <condition_variable>


class ErrorQueue : public IErrorQueue
{
public:
    ~ErrorQueue() override = default;

    void Enqueue(ErrorRecord record) noexcept override;

    bool Dequeue(ErrorRecord& out, DWORD timeoutMs) noexcept override;

    size_t Size() const noexcept override;
    bool Empty() const noexcept override;

    void Clear() noexcept override;

    void Shutdown() noexcept override;

private:
    std::queue<ErrorRecord> _File;
    mutable std::mutex      _Mutex;
    std::condition_variable _Cv;
    bool                    _Running = true;
};