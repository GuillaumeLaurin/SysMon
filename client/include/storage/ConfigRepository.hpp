#pragma once

#include "interfaces/IConfigRepository.hpp"

#include "interfaces/IDatabase.hpp"

#include <memory>

/**
 * @file ConfigRepository.hpp
 * @brief IConfigRepository implementation backed by a SQLite `config` key/value table.
 */

/**
 * @brief IConfigRepository implementation persisting key/value configuration
 *        entries in the `config` table of the underlying SQLite database.
 */
class ConfigRepository : public IConfigRepository
{
public:
    /**
     * @brief Constructs a config repository bound to the given database connection.
     * @param connexion Database connection used to run the config queries.
     */
    ConfigRepository(std::shared_ptr<IDatabase> connexion);

    /** @brief Default destructor. */
    ~ConfigRepository() override = default;

    /**
     * @brief Inserts or updates the value stored for the given key.
     * @param key Configuration key to set.
     * @param value Value to associate with @p key.
     * @return true on success, false if the database is closed or the query failed.
     */
    bool Set(std::string_view key, std::string_view value) noexcept override;

    /**
     * @brief Retrieves the value stored for the given key.
     * @param key Configuration key to look up.
     * @param defaultValue Value returned when the key is missing or on error.
     * @return The stored value, or @p defaultValue if not found.
     */
    std::string Get(std::string_view key, std::string_view defaultValue = "") noexcept override;

    /**
     * @brief Deletes the entry stored for the given key.
     * @param key Configuration key to remove.
     * @return true on success, false if the database is closed or the query failed.
     */
    bool Remove(std::string_view key) noexcept override;

    /**
     * @brief Checks whether an entry exists for the given key.
     * @param key Configuration key to check.
     * @return true if the key exists, false otherwise or on error.
     */
    bool Exists(std::string_view key) const noexcept override;

private:
    /** @brief Database connection used to run the config queries. */
    std::shared_ptr<IDatabase> _Connexion;
};