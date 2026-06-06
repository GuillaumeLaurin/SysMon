#pragma once

#include "FastMutex.h"

/**
 * @brief Holds all the global state of the driver, instead of creating separatge global variables
 */
struct Globals
{
    /**
     * 
     */
    VOID Init(_In_ ULONG maxCount);

    VOID Destroy();

    /**
     * 
     */
    VOID AddItem(_In_ LIST_ENTRY* entry);

    /**
     * 
     */
    VOID AddHeadItem(_In_ LIST_ENTRY* entry);

    /**
     * 
     */
    LIST_ENTRY* RemoveItem();

private:
    LIST_ENTRY _ItemsHead;
    ULONG _Count;
    ULONG _MaxCount;
    FastMutex _Lock;
};