#pragma once

#include "interfaces/IDatabase.hpp"

/**
 * @file Database.hpp
 * @brief IDatabase implementation wrapping a SQLite connection, including schema migration.
 */

/**
 * @brief IDatabase implementation wrapping a raw sqlite3 connection handle.
 *
 * @note Not thread-safe; a single Database instance is expected to be used
 *       from one thread (or externally synchronized) since sqlite3* access
 *       is not serialized here.
 */
class Database : public IDatabase
{
public:
    /** @brief Default destructor; does not close the connection (call Close() explicitly). */
    ~Database() override = default;

    /**
     * @brief Opens (or reopens) the SQLite database at the given path and runs migrations.
     * @param path Filesystem path to the SQLite database file.
     * @return true on success, false if the connection could not be opened or migration failed.
     * @note Closes any previously open connection before opening the new one.
     */
    bool Open(std::string_view path) noexcept override;

    /**
     * @brief Closes the current connection, if open.
     */
    void Close() noexcept override;

    /**
     * @brief Checks whether a connection is currently open.
     * @return true if the underlying sqlite3 handle is non-null.
     */
    bool IsOpen() const noexcept override;

    /**
     * @brief Creates the `events` and `config` tables if they do not already exist.
     * @return true on success, false if the database is closed or the migration query failed.
     */
    bool Migrate() noexcept override;

    /**
     * @brief Returns the raw sqlite3 connection handle.
     * @return The underlying sqlite3*, or nullptr if not open.
     */
    sqlite3* Handle() const noexcept override;

private:
    /** @brief Raw sqlite3 connection handle, or nullptr when closed. */
    sqlite3* _Connexion = nullptr;
};