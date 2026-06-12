#pragma once

#include "FastMutex.h"
#include "ExecutiveResource.h"
#include "LookasideList.h"

/**
 * @brief Holds all the global state of the driver, instead of creating separatge global variables
 */
struct Globals
{
    /**
     * 
     */
    VOID Init(_In_ ULONG maxCount, _In_ ULONG processesMaxCount);

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

    /**
     * 
     */
    bool AddNewProcess(_In_ LIST_ENTRY* entry);

    /**
     * 
     */
    bool RemoveProcess(_In_ HANDLE pid);

    /**
     * 
     */
    VOID ClearNewProcesses();

private:
    // Items 
    // Process, Thread, Image & RemoteThread
    LIST_ENTRY _ItemsHead;
    ULONG      _Count;
    ULONG      _MaxCount;
    FastMutex  _Lock;
    // New Processes
    LIST_ENTRY        _NewProcessesHead;
    ULONG             _NewProcessesCount;
    ULONG             _NewProcessesMaxCount;
    ExecutiveResource _NewProcessesLock;
};