/**
 * @file IDatabase.hpp
 * @brief Contract for the component owning the SQLite connection shared by repositories.
 */

#pragma once

#include <string>

#include <sqlite3.h>

/**
 * @brief Owns the SQLite connection used by the repositories.
 */
class IDatabase
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     * @note Implementations should close the connection if still open.
     */
    virtual ~IDatabase() = default;

    /**
     * @brief Opens (or creates) the database file at @p path.
     * @param path Filesystem path of the SQLite database file.
     * @return true on success, false if the file could not be opened/created.
     */
    virtual bool Open(std::string_view path) noexcept = 0;

    /**
     * @brief Closes the connection if open.
     */
    virtual void Close() noexcept = 0;

    /**
     * @brief Returns true while a connection is open.
     * @return true if the database connection is currently open.
     */
    virtual bool IsOpen() const noexcept = 0;

    /**
     * @brief Creates/upgrades the schema to the current version.
     * @return true on success, false if the migration failed.
     */
    virtual bool Migrate() noexcept = 0;

    /**
     * @brief Raw SQLite handle for repository implementations.
     * @return Pointer to the underlying `sqlite3` connection, or nullptr if not open.
     * @note The returned pointer is owned by the IDatabase implementation and remains
     *       valid only until Close() is called.
     */
    virtual sqlite3* Handle() const noexcept = 0;
};
