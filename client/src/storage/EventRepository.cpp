#include "storage/EventRepository.hpp"

/** @brief Stores the database connection used to run the event queries. */
EventRepository::EventRepository(std::shared_ptr<IDatabase> connexion)
    : _Connexion(connexion)
{
}

/** @brief Runs `INSERT INTO events (id, type, timestamp, pid, tid, data) VALUES (...)` for the given record. */
bool EventRepository::Insert(const EventRecord& record) noexcept
{
    if (!_Connexion->IsOpen())
    {
        return false;
    }

    auto sql = R"(
        INSERT INTO events (id, type, timestamp, pid, tid, data) VALUES (?, ?, ?, ?, ?, ?)
    )";

    sqlite3_stmt* stmt = nullptr;

    auto result = sqlite3_prepare_v2(
        _Connexion->Handle(),
        sql,
        -1,
        &stmt,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, record.Id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, record.Type.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int64(stmt, 3, record.Timestamp.QuadPart) != SQLITE_OK ||
        sqlite3_bind_int64(stmt, 4, record.Pid) != SQLITE_OK ||
        sqlite3_bind_int64(stmt, 5, record.Tid) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 6, record.Data.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return false;
    }
    auto stepResult = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return stepResult == SQLITE_DONE;
}   

/** @brief Runs `SELECT * FROM events` and maps every row to an EventRecord. */
std::vector<EventRecord> EventRepository::QueryAll() noexcept
{
    if (!_Connexion->IsOpen())
    {
        return {};
    }

    auto sql = R"(
        SELECT * FROM events;
    )";

    sqlite3_stmt* stmt = nullptr;

    auto result = sqlite3_prepare_v2(
        _Connexion->Handle(),
        sql,
        -1,
        &stmt,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        return {};
    }

    std::vector<EventRecord> records = {};

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        EventRecord record;

        record.Id        = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        record.Type      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        record.Timestamp.QuadPart = sqlite3_column_int64(stmt, 2);
        record.Pid       = sqlite3_column_int(stmt, 3);
        record.Tid       = sqlite3_column_int(stmt, 4);
        record.Data      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        records.push_back(std::move(record));
    }

    sqlite3_finalize(stmt);
    return records;
}

/** @brief Runs `SELECT * FROM events WHERE type = ?` and maps every matching row to an EventRecord. */
std::vector<EventRecord> EventRepository::QueryByType(std::string_view type) noexcept
{
    if (!_Connexion->IsOpen())
    {
        return {};
    }

    auto sql = R"(
        SELECT * FROM events WHERE type = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    auto result = sqlite3_prepare_v2(
        _Connexion->Handle(),
        sql,
        -1,
        &stmt,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        return {};
    }

    if (sqlite3_bind_text(stmt, 1, type.data(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return {};
    }

    std::vector<EventRecord> records = {};

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        EventRecord record;

        record.Id        = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        record.Type      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        record.Timestamp.QuadPart = sqlite3_column_int64(stmt, 2);
        record.Pid       = sqlite3_column_int(stmt, 3);
        record.Tid       = sqlite3_column_int(stmt, 4);
        record.Data      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        records.push_back(std::move(record));
    }

    sqlite3_finalize(stmt);
    return records;
}

/** @brief Runs `SELECT * FROM events WHERE pid = ?` and maps every matching row to an EventRecord. */
std::vector<EventRecord> EventRepository::QueryByPid(ULONG pid) noexcept
{
    if (!_Connexion->IsOpen())
    {
        return {};
    }

    auto sql = R"(
        SELECT * FROM events WHERE pid = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    auto result = sqlite3_prepare_v2(
        _Connexion->Handle(),
        sql,
        -1,
        &stmt,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        return {};
    }

    if (sqlite3_bind_int64(stmt, 1, pid) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return {};
    }

    std::vector<EventRecord> records = {};

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        EventRecord record;

        record.Id        = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        record.Type      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        record.Timestamp.QuadPart = sqlite3_column_int64(stmt, 2);
        record.Pid       = sqlite3_column_int(stmt, 3);
        record.Tid       = sqlite3_column_int(stmt, 4);
        record.Data      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        records.push_back(std::move(record));
    }

    sqlite3_finalize(stmt);
    return records;
}

/** @brief Runs `DELETE FROM events` via sqlite3_exec. */
bool EventRepository::Clear() noexcept
{
    if (!_Connexion->IsOpen())
    {
        return false;
    }

    auto sql = R"(
        DELETE FROM events;
    )";

    auto result = sqlite3_exec(_Connexion->Handle(), sql, nullptr, nullptr, nullptr);
    return result == SQLITE_OK;
}