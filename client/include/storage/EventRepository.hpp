#pragma once

#include "interfaces/IEventRepository.hpp"

#include "interfaces/IDatabase.hpp"

#include <memory>

/**
 * @file EventRepository.hpp
 * @brief IEventRepository implementation backed by the SQLite `events` table.
 */

/**
 * @brief IEventRepository implementation persisting and querying EventRecord
 *        rows in the `events` table of the underlying SQLite database.
 */
class EventRepository : public IEventRepository
{
public:
    /**
     * @brief Constructs an event repository bound to the given database connection.
     * @param connexion Database connection used to run the event queries.
     */
    EventRepository(std::shared_ptr<IDatabase> connexion);

    /** @brief Default destructor. */
    ~EventRepository() override = default;

    /**
     * @brief Inserts a new event record.
     * @param record Event to persist.
     * @return true on success, false if the database is closed or the insert failed.
     */
    bool Insert(const EventRecord& record) noexcept override;

    /**
     * @brief Retrieves every stored event record.
     * @return All rows in the `events` table, or an empty vector if the database is closed or the query failed.
     */
    std::vector<EventRecord> QueryAll() noexcept override;

    /**
     * @brief Retrieves the event records matching the given type.
     * @param type Event type to filter by.
     * @return Matching rows, or an empty vector if the database is closed, the query failed, or no rows match.
     */
    std::vector<EventRecord> QueryByType(std::string_view type) noexcept override;

    /**
     * @brief Retrieves the event records matching the given process id.
     * @param pid Process identifier to filter by.
     * @return Matching rows, or an empty vector if the database is closed, the query failed, or no rows match.
     */
    std::vector<EventRecord> QueryByPid(ULONG pid) noexcept override;

    /**
     * @brief Deletes every stored event record.
     * @return true on success, false if the database is closed or the delete failed.
     */
    bool Clear() noexcept override;

private:
    /** @brief Database connection used to run the event queries. */
    std::shared_ptr<IDatabase> _Connexion;
};