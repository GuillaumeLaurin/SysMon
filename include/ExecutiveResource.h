#pragma once

/**
 * @file ExecutiveResource.h
 * @brief Declares ExecutiveResource, a thin wrapper around ERESOURCE offering
 *        exclusive and shared locking with automatic critical-region handling.
 */

#ifdef USE_KMDF
#include <wdf.h>
#else
#include <wdm.h>
#endif

/**
 * @brief Wrapper around ERESOURCE providing exclusive and shared locking.
 *        Automatically enters/leaves a critical region when APCs are not
 *        already disabled at acquisition time.
 */
struct ExecutiveResource
{
    /**
     * @brief Initializes the underlying ERESOURCE. Must be called before any locking.
     */
    void Init();

    /**
     * @brief Deletes the underlying ERESOURCE. No lock may be held.
     */
    void Delete();

    /**
     * @brief Acquires the resource exclusively (blocking).
     */
    void Lock();

    /**
     * @brief Releases an exclusive acquisition.
     */
    void Unlock();

    /**
     * @brief Acquires the resource in shared mode (blocking).
     */
    void LockShared();

    /**
     * @brief Releases a shared acquisition.
     */
    void UnlockShared();

private:
    ERESOURCE _Res;
    bool      _CritRegion;
};
