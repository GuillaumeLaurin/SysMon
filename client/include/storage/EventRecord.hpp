#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <string>

/**
 * @file EventRecord.hpp
 * @brief DTO representing a single system event captured by the driver and
 *        persisted through IEventRepository.
 */

/**
 * @brief Plain data transfer object describing one system event captured by
 *        the kernel driver and stored in (or read from) the `events` table.
 */
struct EventRecord
{
    /** @brief Unique identifier of the event (primary key in storage). */
    std::string   Id;

    /** @brief Event type/category, as reported by the driver. */
    std::string   Type;

    /** @brief Timestamp at which the event was captured. */
    LARGE_INTEGER Timestamp;

    /** @brief Identifier of the process associated with the event. */
    ULONG         Pid;

    /** @brief Identifier of the thread associated with the event. */
    ULONG         Tid;

    /** @brief Additional event payload/details, as free-form text. */
    std::string   Data;
};