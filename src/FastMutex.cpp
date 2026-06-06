#ifdef USE_KMDF

#include "Common.h"
#include "FastMutex.h"

VOID FastMutex::Init()
{
    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

    NTSTATUS status = WdfWaitLockCreate(&attributes, &_Mutex);
    UNREFERENCED_PARAMETER(status);
    NT_ASSERT(NT_SUCCESS(status));
}

VOID FastMutex::Destroy()
{
    if (_Mutex != nullptr) {
        WdfObjectDelete(_Mutex);
        _Mutex = nullptr;
    }
}

VOID FastMutex::Lock()
{
    WdfWaitLockAcquire(_Mutex, NULL);
}

VOID FastMutex::Unlock()
{
    WdfWaitLockRelease(_Mutex);
}

#else // !USE_KMDF

#include "FastMutex.h"

VOID FastMutex::Init()
{
    ExInitializeFastMutex(&_Mutex);
}

VOID FastMutex::Destroy()
{
    // FAST_MUTEX does not require explicit destruction
}

VOID FastMutex::Lock()
{
    ExAcquireFastMutex(&_Mutex);
}

VOID FastMutex::Unlock()
{
    ExReleaseFastMutex(&_Mutex);
}

#endif // USE_KMDF