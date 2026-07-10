#pragma once

#include "interfaces/IEventProcessor.hpp"

#include "interfaces/IEventRepository.hpp"

#include "interfaces/IDriverConnector.hpp"

#include "interfaces/IErrorDispatcher.hpp"

#include <atomic>
#include <thread>
#include <memory>

/**
 * @file EventProcessor.hpp
 * @brief Declares the EventProcessor background worker used to poll the
 *        SysMon driver and persist decoded kernel events.
 */

struct RegistrySetValueInfo;

/**
 * @brief Background worker polling the driver for raw events, decoding them
 *        (Public.h structs) and persisting them through the event repository.
 */
class EventProcessor : public IEventProcessor
{
public:
    /**
     * @brief Constructs the event processor with its dependencies.
     * @param connector Driver connector used to read raw events.
     * @param repository Repository used to persist decoded events.
     * @param dispatcher Dispatcher used to report errors.
     */
    EventProcessor(
        std::shared_ptr<IDriverConnector> connector,
        std::shared_ptr<IEventRepository> repository,
        std::shared_ptr<IErrorDispatcher> dispatcher
    );

    /** @brief Stops the worker thread if still running. */
    ~EventProcessor() override;

    /** @brief Starts the polling thread. */
    void Start() noexcept override;

    /** @brief Signals the polling thread to stop and joins it. */
    void Stop() noexcept override;

private:
    /** @brief Connector used to read raw events from the driver. */
    std::shared_ptr<IDriverConnector> _Connector;
    /** @brief Repository used to persist decoded events. */
    std::shared_ptr<IEventRepository> _Repository;
    /** @brief Dispatcher used to report processing errors. */
    std::shared_ptr<IErrorDispatcher> _Dispatcher;
    /** @brief Thread running the polling loop. */
    std::thread                       _Worker;
    /** @brief Flag controlling whether the worker loop keeps running. */
    std::atomic<bool>                 _Running = false;

    /** @brief Polling loop: reads from the driver and hands buffers to ProcessBuffer(). */
    void WorkerLoop();

    /**
     * @brief Walks a raw driver buffer and converts each ItemHeader-based event to an EventRecord.
     * @param buffer Raw buffer filled by the driver read.
     * @param size Number of valid bytes in @p buffer.
     */
    void ProcessBuffer(BYTE* buffer, DWORD size);

    /**
     * @brief Converts an NT device path (\\Device\\HarddiskVolumeX\\...) to its DOS drive form.
     * @param path NT device path to convert.
     * @return The DOS-style path, or @p path unchanged if it cannot be resolved.
     */
    std::wstring GetDosNameFromNTName(PCWSTR path);

    /**
     * @brief UTF-16 to UTF-8 conversion helper.
     * @param wstr Wide string to convert.
     * @return The UTF-8 encoded equivalent of @p wstr.
     */
    std::string WStringToString(const std::wstring& wstr);

    /**
     * @brief Decodes the value payload of a RegistrySetValue event according to its REG_* type.
     * @param info Registry event whose trailing data is decoded (at most ProvidedDataSize bytes).
     * @return Human-readable representation of the value (hex dump for binary types).
     */
    std::wstring RegistryValue(const RegistrySetValueInfo* info);

    /**
     * @brief Maps a REG_* data type constant to its symbolic name.
     * @param type REG_* constant reported by the driver.
     * @return The constant's name (e.g. L"REG_SZ"), or L"REG_UNKNOWN(n)".
     */
    std::wstring RegistryTypeName(ULONG type);

    /**
     * @brief Formats a raw byte buffer as a space-separated hex string.
     * @param buffer Bytes to format.
     * @param size Number of bytes in @p buffer.
     * @return The hex representation of the buffer.
     */
    std::wstring ToBinary(const BYTE* buffer, DWORD size);
};
