/**
 * @file ErrorQueue.hpp
 * @brief Thread-safe FIFO queue of ErrorRecord used to hand records off from
 *        producer threads to the ErrorDispatcher worker thread.
 */

#pragma once

#include "interfaces/IErrorQueue.hpp"

#include <queue>

#include <mutex>

#include <condition_variable>

/**
 * @brief IErrorQueue implementation based on std::queue plus a condition
 *        variable for blocking waits with timeout.
 * @note Thread-safe: all operations are internally synchronized with a mutex,
 *       so producers and the consumer can call them concurrently.
 */
class ErrorQueue : public IErrorQueue
{
public:
    /** @brief Default destructor; no special teardown is required. */
    ~ErrorQueue() override = default;

    /**
     * @brief Pushes a record and wakes one waiting consumer.
     * @param record Error record to enqueue (moved in).
     */
    void Enqueue(ErrorRecord record) noexcept override;

    /**
     * @brief Pops a record, waiting up to @p timeoutMs for one to become available.
     * @param out Receives the dequeued record on success.
     * @param timeoutMs Maximum time to wait, in milliseconds.
     * @return true if a record was dequeued; false on timeout or after Shutdown().
     */
    bool Dequeue(ErrorRecord& out, DWORD timeoutMs) noexcept override;

    /**
     * @brief Number of pending records.
     * @return Current queue size.
     */
    size_t Size() const noexcept override;

    /**
     * @brief Returns true when no record is pending.
     * @return true if the queue is empty.
     */
    bool Empty() const noexcept override;

    /** @brief Discards all pending records. */
    void Clear() noexcept override;

    /**
     * @brief Wakes every waiting consumer and makes further Dequeue() calls fail fast.
     * @note Does not clear pending records; it only unblocks waiters and flips
     *       the running flag so blocking waits return immediately.
     */
    void Shutdown() noexcept override;

private:
    std::queue<ErrorRecord> _Queue;   ///< Backing FIFO storage.
    mutable std::mutex      _Mutex;   ///< Guards all access to _Queue and _Running.
    std::condition_variable _Cv;      ///< Signaled on Enqueue()/Shutdown() to wake waiters.
    bool                    _Running = true; ///< false once Shutdown() has been called.
};
