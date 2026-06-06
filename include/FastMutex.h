#pragma once

#ifdef USE_KMDF
#include <wdf.h>
#else
#include <wdm.h>
#endif

struct FastMutex
{
    VOID Init();

    VOID Lock();

    VOID Unlock();

private:
    #ifdef USE_KMDF
    WDFWAITLOCK _Mutex;
    #else // !USE_KMDF
    FAST_MUTEX _Mutex;
    #endif // USE_KMDF
};