#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "interfaces/IErrorFilter.hpp"

#include <unordered_map>

class RateLimitFilter : public IErrorFilter
{
    struct RateLimitEntry
    {
        ULONGLONG WindowStart;
        ULONG     Count;
    };
public:
    RateLimitFilter(ULONG maxCount, ULONGLONG timeSpanMs);

    ~RateLimitFilter() override = default;

    bool ShouldProcess(const ErrorRecord& record) const noexcept override;

private:
    mutable std::unordered_map<std::string, RateLimitEntry> _Entries;
    ULONG                                                   _MaxCount;
    ULONGLONG                                               _TimeSpanMs;
};