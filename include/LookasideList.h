#pragma once

#ifdef USE_KMDF
#include <wdf.h>
#else
#include <wdm.h>
#endif

template <typename T>
struct LookasideList
{
    NTSTATUS Init(
        _In_ POOL_TYPE pool,
        _In_ ULONG     tag
    )
    {
        return ExInitializeLookasideListEx(&_Lookaside, nullptr, nullptr, 
            pool, 0, sizeof(T), tag, 0);
    }

    void Delete()
    {
        ExDeleteLookasideListEx(&_Lookaside);
    }

    T* Alloc()
    {
        return (T*)ExAllocateFromLookasideListEx(_Lookaside);
    }

    void Free(T* p)
    {
        ExFreeToLookasideListEx(&_Lookaside, p);
    }

private:
    LOOKASIDE_LIST_EX _Lookaside;
};