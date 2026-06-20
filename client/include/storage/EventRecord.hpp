#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <string>

struct EventRecord
{
    std::string   Id;
    std::string   Type;
    LARGE_INTEGER Timestamp;
    ULONG         Pid;
    ULONG         Tid;
    std::string   Data;
};