#pragma once

/**
 * @file ConfigKeys.hpp
 * @brief Central list of configuration key names (and their default values)
 *        persisted through IConfigRepository.
 */

/**
 * @brief Configuration keys (and default values) persisted through IConfigRepository.
 */
namespace ConfigKeys
{
    /** @brief Key for the UI refresh period, in milliseconds. */
    inline constexpr const char* RefreshIntervalMs = "ui.refreshIntervalMs";

    /** @brief Key for the maximum number of rows shown in event tables. */
    inline constexpr const char* MaxRows           = "ui.maxRows";

    /** @brief Default value (as string) used when RefreshIntervalMs is not set. */
    inline constexpr const char* DefaultRefreshIntervalMs = "500";

    /** @brief Default value (as string) used when MaxRows is not set. */
    inline constexpr const char* DefaultMaxRows           = "1000";
}
