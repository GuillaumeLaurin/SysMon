#include "reporter/ErrorQueue.hpp"

#include <chrono>

void ErrorQueue::Enqueue(ErrorRecord record) noexcept
{
    {   
        auto lock = std::lock_guard(_Mutex);
        _File.push(std::move(record));
    }
    _Cv.notify_one();
}

bool ErrorQueue::Dequeue(ErrorRecord& out, DWORD timeoutMs) noexcept
{
    auto lock = std::unique_lock(_Mutex);
    
    auto status = _Cv.wait_for(lock, std::chrono::milliseconds(timeoutMs), [&](){
        return !_File.empty() || !_Running;
    });

    if (!status || _File.empty())
    {
        return false;
    }

    out = std::move(_File.front());
    _File.pop();

    return true;
}

size_t ErrorQueue::Size() const noexcept
{
    auto lock = std::lock_guard(_Mutex);
    return _File.size();
}

bool ErrorQueue::Empty() const noexcept
{
    auto lock = std::lock_guard(_Mutex);
    return _File.empty();
}

void ErrorQueue::Clear() noexcept
{
    auto lock = std::lock_guard(_Mutex);
    std::queue<ErrorRecord> empty;
    std::swap(_File, empty);
}

void ErrorQueue::Shutdown() noexcept
{
    {
        auto lock = std::lock_guard(_Mutex);
        _Running = false;
    }
    _Cv.notify_all();
}