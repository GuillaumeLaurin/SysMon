#include "storage/Database.hpp"

/** @brief Closes any existing connection, opens the SQLite file at @p path via sqlite3_open, then runs Migrate(). */
bool Database::Open(std::string_view path) noexcept
{
    if (IsOpen())
    {
        Close();
    }

    auto result = sqlite3_open(path.data(), &_Connexion);

    if (result != SQLITE_OK)
    {
        _Connexion = nullptr;
        return false;
    }

    return Migrate();
}

/** @brief Closes the connection via sqlite3_close and resets the handle, if currently open. */
void Database::Close() noexcept
{
    if (!IsOpen())
    {
        return;
    }

    sqlite3_close(_Connexion);
    _Connexion = nullptr;
}

/** @brief Returns whether the underlying sqlite3 handle is non-null. */
bool Database::IsOpen() const noexcept
{
    return _Connexion != nullptr;
}

/** @brief Runs `CREATE TABLE IF NOT EXISTS` for the `events` and `config` tables via sqlite3_exec. */
bool Database::Migrate() noexcept
{
    if (!IsOpen())
    {
        return false;
    }

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS events (
            id          TEXT PRIMARY KEY,
            type        TEXT NOT NULL,
            timestamp   INTEGER NOT NULL,
            pid         INTEGER,
            tid         INTEGER,
            data        TEXT
        );
        CREATE TABLE IF NOT EXISTS config (
            key         TEXT PRIMARY KEY,
            value       TEXT NOT NULL
        );
    )";

    auto result = sqlite3_exec(_Connexion, sql.c_str(), nullptr, nullptr, nullptr);

    return result == SQLITE_OK;
}

/** @brief Returns the raw sqlite3 connection handle. */
sqlite3* Database::Handle() const noexcept
{
    return _Connexion;
}