/**
 * @file IErrorQueue.hpp
 * @brief Contract for the thread-safe queue feeding the error dispatcher worker.
 */

#pragma once

#include "reporter/ErrorRecord.hpp"

/**
 * @brief Thread-safe producer/consumer queue of error records feeding the dispatcher worker.
 *
 * @note Implementations must support concurrent Enqueue() from multiple producer threads
 *       and Dequeue() from a single consumer thread.
 */
class IErrorQueue
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     * @note Implementations should call Shutdown() if not already done.
     */
    virtual ~IErrorQueue() = default;

    /**
     * @brief Pushes a record (producer side, never blocks).
     * @param record Error record to enqueue; taken by value and moved into the queue.
     */
    virtual void Enqueue(ErrorRecord record) noexcept = 0;

    /**
     * @brief Pops a record, waiting up to @p timeoutMs; returns false on timeout or shutdown.
     * @param out Receives the dequeued record on success.
     * @param timeoutMs Maximum time to wait for a record, in milliseconds.
     * @return true if a record was dequeued into @p out, false on timeout or after Shutdown().
     */
    virtual bool Dequeue(ErrorRecord& out, DWORD timeoutMs) noexcept = 0;

    /**
     * @brief Number of pending records.
     * @return Count of records currently queued.
     */
    virtual size_t Size() const noexcept = 0;

    /**
     * @brief Returns true when no record is pending.
     * @return true if the queue currently holds no record.
     */
    virtual bool Empty() const noexcept = 0;

    /**
     * @brief Discards all pending records.
     */
    virtual void Clear() noexcept = 0;

    /**
     * @brief Wakes up waiting consumers and rejects further blocking waits.
     * @note After Shutdown(), Dequeue() must return promptly with false.
     */
    virtual void Shutdown() noexcept = 0;
};
