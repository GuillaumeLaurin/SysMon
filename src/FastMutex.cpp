/**
 * @file FastMutex.cpp
 * @brief Implements FastMutex, declared in FastMutex.h, backed by
 *        WDFWAITLOCK (KMDF) or FAST_MUTEX (WDM).
 */

#ifdef USE_KMDF

#include "Common.h"
#include "FastMutex.h"

/**
 * @brief Creates the underlying WDFWAITLOCK object.
 */
VOID FastMutex::Init()
{
    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

    NTSTATUS status = WdfWaitLockCreate(&attributes, &_Mutex);
    UNREFERENCED_PARAMETER(status);
    NT_ASSERT(NT_SUCCESS(status));
}

/**
 * @brief Deletes the WDFWAITLOCK object, if created.
 */
VOID FastMutex::Destroy()
{
    if (_Mutex != nullptr) {
        WdfObjectDelete(_Mutex);
        _Mutex = nullptr;
    }
}

/**
 * @brief Acquires the WDFWAITLOCK (blocking).
 */
VOID FastMutex::Lock()
{
    WdfWaitLockAcquire(_Mutex, NULL);
}

/**
 * @brief Releases the WDFWAITLOCK.
 */
VOID FastMutex::Unlock()
{
    WdfWaitLockRelease(_Mutex);
}

#else // !USE_KMDF

#include "FastMutex.h"

/**
 * @brief Initializes the underlying FAST_MUTEX.
 */
VOID FastMutex::Init()
{
    ExInitializeFastMutex(&_Mutex);
}

/**
 * @brief No-op: FAST_MUTEX does not require explicit destruction.
 */
VOID FastMutex::Destroy()
{
    // FAST_MUTEX does not require explicit destruction
}

/**
 * @brief Acquires the FAST_MUTEX (blocking).
 *
 * @note Raises IRQL to APC_LEVEL for the duration of the acquisition.
 */
VOID FastMutex::Lock()
{
    ExAcquireFastMutex(&_Mutex);
}

/**
 * @brief Releases the FAST_MUTEX.
 */
VOID FastMutex::Unlock()
{
    ExReleaseFastMutex(&_Mutex);
}

#endif // USE_KMDF