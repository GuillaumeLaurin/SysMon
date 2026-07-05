/**
 * @file RateLimitFilter.cpp
 * @brief Implementation of RateLimitFilter's sliding-window rate limiting.
 */

#include "reporter/RateLimitFilter.hpp"

/** @brief Stores the configured max count and window length. */
RateLimitFilter::RateLimitFilter(ULONG maxCount, ULONGLONG timeSpanMs)
    : _MaxCount(maxCount), _TimeSpanMs(timeSpanMs)
{
}

/**
 * @brief Looks up (or creates) the fingerprint's window entry, resets it if
 *        the window has elapsed, then accepts or rejects based on _MaxCount.
 * @note See the @warning on the class declaration regarding thread-safety.
 */
bool RateLimitFilter::ShouldProcess(const ErrorRecord& record) const noexcept
{
    auto now = GetTickCount64();
    std::string fingerprint = record.Fingerprint;

    auto it = _Entries.find(fingerprint);

    if (it == _Entries.end())
    {
        RateLimitEntry entry;
        entry.WindowStart = now;
        entry.Count       = 1;
        _Entries[fingerprint] = entry;
        return true;
    }

    auto& entry = it->second;

    if (now - entry.WindowStart >= _TimeSpanMs)
    {
        entry.WindowStart = now;
        entry.Count = 1;
        return true;
    }

    if (entry.Count >= _MaxCount)
    {
        return false;
    }

    entry.Count++;
    return true;
}