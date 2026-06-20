#include "reporter/RateLimitFilter.hpp"

RateLimitFilter::RateLimitFilter(ULONG maxCount, ULONGLONG timeSpanMs)
    : _MaxCount(maxCount), _TimeSpanMs(timeSpanMs)
{
}

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