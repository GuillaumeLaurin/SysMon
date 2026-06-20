#include "storage/Database.hpp"

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

void Database::Close() noexcept
{
    if (!IsOpen())
    {
        return;
    }

    sqlite3_close(_Connexion);
    _Connexion = nullptr;
}

bool Database::IsOpen() const noexcept
{
    return _Connexion != nullptr;
}

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

sqlite3* Database::Handle() const noexcept
{
    return _Connexion;
}