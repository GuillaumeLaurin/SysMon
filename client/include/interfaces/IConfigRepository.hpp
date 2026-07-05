/**
 * @file IConfigRepository.hpp
 * @brief Contract for the persistent key/value store backing application settings.
 */

#pragma once

#include <string>

/**
 * @brief Persistent key/value store for application settings.
 *
 * @note All operations are noexcept; implementations must translate storage
 *       failures (e.g. database errors) into a `false`/default-value return
 *       rather than throwing.
 */
class IConfigRepository
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IConfigRepository() = default;

    /**
     * @brief Stores (inserts or updates) a value under the given key.
     * @param key Configuration key.
     * @param value Value to persist under @p key.
     * @return true on success, false if the value could not be persisted.
     */
    virtual bool Set(std::string_view key, std::string_view value) noexcept = 0;

    /**
     * @brief Returns the value stored under @p key, or @p defaultValue when absent.
     * @param key Configuration key to look up.
     * @param defaultValue Value returned when @p key does not exist.
     * @return The stored value, or @p defaultValue when the key is absent.
     */
    virtual std::string Get(std::string_view key, std::string_view defaultValue = "") noexcept = 0;

    /**
     * @brief Deletes the entry with the given key.
     * @param key Configuration key to remove.
     * @return true if an entry was removed, false if it did not exist or removal failed.
     */
    virtual bool Remove(std::string_view key) noexcept = 0;

    /**
     * @brief Returns true if an entry exists for the given key.
     * @param key Configuration key to check.
     * @return true if the key is present in the repository.
     */
    virtual bool Exists(std::string_view key) const noexcept = 0;
};
