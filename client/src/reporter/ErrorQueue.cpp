/**
 * @file ErrorQueue.cpp
 * @brief Implementation of the mutex/condition_variable-based ErrorQueue.
 */

#include "reporter/ErrorQueue.hpp"

#include <chrono>

/** @brief Pushes the record under lock, then notifies one waiting consumer. */
void ErrorQueue::Enqueue(ErrorRecord record) noexcept
{
    {   
        auto lock = std::lock_guard(_Mutex);
        _Queue.push(std::move(record));
    }
    _Cv.notify_one();
}

/** @brief Waits on the condition variable until a record is available, timeout elapses, or Shutdown() is called. */
bool ErrorQueue::Dequeue(ErrorRecord& out, DWORD timeoutMs) noexcept
{
    auto lock = std::unique_lock(_Mutex);
    
    auto status = _Cv.wait_for(lock, std::chrono::milliseconds(timeoutMs), [&](){
        return !_Queue.empty() || !_Running;
    });

    if (!status || _Queue.empty())
    {
        return false;
    }

    out = std::move(_Queue.front());
    _Queue.pop();

    return true;
}

/** @brief Returns the current queue size under lock. */
size_t ErrorQueue::Size() const noexcept
{
    auto lock = std::lock_guard(_Mutex);
    return _Queue.size();
}

/** @brief Returns whether the queue is empty under lock. */
bool ErrorQueue::Empty() const noexcept
{
    auto lock = std::lock_guard(_Mutex);
    return _Queue.empty();
}

/** @brief Swaps in an empty queue under lock, discarding all pending records. */
void ErrorQueue::Clear() noexcept
{
    auto lock = std::lock_guard(_Mutex);
    std::queue<ErrorRecord> empty;
    std::swap(_Queue, empty);
}

/** @brief Flips the running flag under lock and wakes all waiters via notify_all(). */
void ErrorQueue::Shutdown() noexcept
{
    {
        auto lock = std::lock_guard(_Mutex);
        _Running = false;
    }
    _Cv.notify_all();
}