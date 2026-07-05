#pragma once

/**
 * @file LookasideList.h
 * @brief Declares LookasideList<T>, a typed wrapper around LOOKASIDE_LIST_EX
 *        for fast fixed-size pool allocations.
 */

#ifdef USE_KMDF
#include <wdf.h>
#else
#include <wdm.h>
#endif

/**
 * @brief Wrapper around LOOKASIDE_LIST_EX for fast fixed-size allocations of T.
 *        Init() must succeed before any Alloc()/Free(), and Delete() must be
 *        called before the owning object goes away.
 *
 * @tparam T Element type; every allocation is sizeof(T)
 */
template <typename T>
struct LookasideList
{
    /**
     * @brief Initializes the lookaside list.
     *
     * @param pool Pool type backing the list (PagedPool / NonPagedPoolNx)
     * @param tag  Pool tag used for the allocations
     * @return NTSTATUS
     */
    NTSTATUS Init(
        _In_ POOL_TYPE pool,
        _In_ ULONG     tag
    )
    {
        return ExInitializeLookasideListEx(&_Lookaside, nullptr, nullptr,
            pool, 0, sizeof(T), tag, 0);
    }

    /**
     * @brief Tears down the lookaside list and returns its memory to the system.
     */
    void Delete()
    {
        ExDeleteLookasideListEx(&_Lookaside);
    }

    /**
     * @brief Allocates one element from the list.
     *
     * @return Pointer to an uninitialized T, or nullptr on failure
     */
    T* Alloc()
    {
        return (T*)ExAllocateFromLookasideListEx(&_Lookaside);
    }

    /**
     * @brief Returns an element previously obtained from Alloc() to the list.
     *
     * @param p Element to free
     */
    void Free(T* p)
    {
        ExFreeToLookasideListEx(&_Lookaside, p);
    }

private:
    LOOKASIDE_LIST_EX _Lookaside;
};
