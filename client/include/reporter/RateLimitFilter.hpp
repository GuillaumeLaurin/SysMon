/**
 * @file RateLimitFilter.hpp
 * @brief IErrorFilter implementation providing flood protection by capping
 *        how many records with the same fingerprint may pass per time window.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "interfaces/IErrorFilter.hpp"

#include <unordered_map>

/**
 * @brief Filter limiting how many records with the same fingerprint pass
 *        within a sliding time window (flood protection).
 * @warning Not thread-safe: _Entries is mutated in ShouldProcess() (a const
 *          method) without any synchronization. If this filter is shared
 *          across threads (e.g. registered on ErrorDispatcher, whose worker
 *          thread invokes filters), concurrent calls are a data race.
 */
class RateLimitFilter : public IErrorFilter
{
    /** @brief Per-fingerprint sliding-window state. */
    struct RateLimitEntry
    {
        ULONGLONG WindowStart; ///< Tick count at the start of the current window
        ULONG     Count;       ///< Records seen in the current window
    };
public:
    /**
     * @brief Configures the rate limit.
     * @param maxCount Maximum number of records allowed per fingerprint within a window.
     * @param timeSpanMs Length of the sliding window, in milliseconds.
     */
    RateLimitFilter(ULONG maxCount, ULONGLONG timeSpanMs);

    /** @brief Default destructor. */
    ~RateLimitFilter() override = default;

    /**
     * @brief Returns false once the fingerprint exceeded its budget for the current window.
     * @param record Record to evaluate; record.Fingerprint identifies its window bucket.
     * @return true if the record is still within budget for its fingerprint's current window.
     * @note Uses GetTickCount64() as the time source, so windows are based on
     *       system uptime rather than wall-clock time.
     */
    bool ShouldProcess(const ErrorRecord& record) const noexcept override;

private:
    mutable std::unordered_map<std::string, RateLimitEntry> _Entries;   ///< Sliding-window state per fingerprint.
    ULONG                                                   _MaxCount;   ///< Max records allowed per window.
    ULONGLONG                                               _TimeSpanMs; ///< Window length, in milliseconds.
};
