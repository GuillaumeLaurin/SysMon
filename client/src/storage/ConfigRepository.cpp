#include "storage/ConfigRepository.hpp"

/** @brief Stores the database connection used to run the config queries. */
ConfigRepository::ConfigRepository(std::shared_ptr<IDatabase> connexion)
    : _Connexion(connexion)
{
}

/** @brief Runs `INSERT OR REPLACE INTO config (key, value) VALUES (?, ?)` to upsert the entry. */
bool ConfigRepository::Set(std::string_view key, std::string_view value) noexcept
{
    if (!_Connexion->IsOpen())
    {
        return false;
    }

    auto sql = R"(
        INSERT OR REPLACE INTO config (key, value) VALUES (?, ?);
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

    if (sqlite3_bind_text(stmt, 1, key.data(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, value.data(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return false;
    }
    auto stepResult = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return stepResult == SQLITE_DONE;
}

/** @brief Runs `SELECT value FROM config WHERE key = ?` and returns defaultValue if no row matches. */
std::string ConfigRepository::Get(std::string_view key, std::string_view defaultValue) noexcept
{
    if (!_Connexion->IsOpen())
    {
        return std::string(defaultValue);
    }

    auto sql = R"(
        SELECT value FROM config WHERE key = ?;
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
        return std::string(defaultValue);
    }

    if (sqlite3_bind_text(stmt, 1, key.data(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return std::string(defaultValue);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        auto value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return value;
    }

    sqlite3_finalize(stmt);
    return std::string(defaultValue);
}

/** @brief Runs `DELETE FROM config WHERE key = ?`. */
bool ConfigRepository::Remove(std::string_view key) noexcept
{
    if (!_Connexion->IsOpen())
    {
        return false;
    }

    auto sql = R"(
        DELETE FROM config WHERE key = ?;
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

    if (sqlite3_bind_text(stmt, 1, key.data(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return false;
    }
    auto stepResult = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return stepResult == SQLITE_DONE;
}

/** @brief Runs `SELECT 1 FROM config WHERE key = ?` and reports whether a row was returned. */
bool ConfigRepository::Exists(std::string_view key) const noexcept
{
    if (!_Connexion->IsOpen())
    {
        return false;
    }

    auto sql = R"(
        SELECT 1 FROM config WHERE key = ?;
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

    if (sqlite3_bind_text(stmt, 1, key.data(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return false;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}