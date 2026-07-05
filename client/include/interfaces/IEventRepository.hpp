/**
 * @file IEventRepository.hpp
 * @brief Contract for persistence and querying of captured kernel events.
 */

#pragma once

#include "storage/EventRecord.hpp"

#include <vector>

/**
 * @brief Persistence and querying of captured kernel events.
 */
class IEventRepository
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IEventRepository() = default;

    /**
     * @brief Persists one event record.
     * @param record Event record to store.
     * @return true on success, false if the record could not be persisted.
     */
    virtual bool Insert(const EventRecord& record) noexcept = 0;

    /**
     * @brief Returns every stored event.
     * @return All event records currently persisted.
     */
    virtual std::vector<EventRecord> QueryAll() noexcept = 0;

    /**
     * @brief Returns the events matching the given type tag.
     * @param type Event type tag to filter on.
     * @return The stored events whose type matches @p type.
     */
    virtual std::vector<EventRecord> QueryByType(std::string_view type) noexcept = 0;

    /**
     * @brief Returns the events attached to the given process ID.
     * @param pid Process identifier to filter on.
     * @return The stored events associated with @p pid.
     */
    virtual std::vector<EventRecord> QueryByPid(ULONG pid) noexcept = 0;

    /**
     * @brief Deletes every stored event.
     * @return true on success, false if the events could not be cleared.
     */
    virtual bool Clear() noexcept = 0;
};
