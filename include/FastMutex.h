#pragma once

/**
 * @file FastMutex.h
 * @brief Declares FastMutex, a mutual-exclusion lock usable below
 *        DISPATCH_LEVEL, backed by WDFWAITLOCK (KMDF) or FAST_MUTEX (WDM).
 */

#ifdef USE_KMDF
#include <wdf.h>
#else
#include <wdm.h>
#endif

/**
 * @brief Mutual-exclusion lock usable at IRQL < DISPATCH_LEVEL.
 *        Wraps a WDFWAITLOCK under KMDF and a FAST_MUTEX under WDM.
 */
struct FastMutex
{
    /**
     * @brief Initializes the underlying lock. Must be called before Lock()/Unlock().
     */
    VOID Init();

    /**
     * @brief Destroys the underlying lock (no-op for FAST_MUTEX).
     */
    VOID Destroy();

    /**
     * @brief Acquires the mutex (blocking).
     */
    VOID Lock();

    /**
     * @brief Releases the mutex.
     */
    VOID Unlock();

private:
    #ifdef USE_KMDF
    WDFWAITLOCK _Mutex;
    #else // !USE_KMDF
    FAST_MUTEX _Mutex;
    #endif // USE_KMDF
};
