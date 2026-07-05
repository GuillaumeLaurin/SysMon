/**
 * @file IEventProcessor.hpp
 * @brief Contract for the background service turning raw driver events into stored records.
 */

#pragma once

/**
 * @brief Background service that pulls raw events from the driver,
 *        parses them and persists them to storage.
 *
 * @note Start()/Stop() drive an internal worker thread; implementations are
 *       expected to make Stop() safe to call even if Start() was never invoked.
 */
class IEventProcessor
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     * @note Implementations should call Stop() if the worker thread is still running.
     */
    virtual ~IEventProcessor() = default;

    /**
     * @brief Starts the worker thread.
     */
    virtual void Start() noexcept = 0;

    /**
     * @brief Stops the worker thread and waits for it to finish.
     */
    virtual void Stop() noexcept = 0;
};
