#pragma once

/**
 * @file Globals.h
 * @brief Declares Globals, the single struct owning all driver-wide state:
 *        the pending event-item list and the tracked new-processes list.
 */

#include "FastMutex.h"
#include "ExecutiveResource.h"
#include "LookasideList.h"

/**
 * @brief Holds all the global state of the driver, instead of creating separate global variables.
 *
 * Owns two intrusive lists:
 *  - the event item list (process / thread / image / remote-thread events),
 *    bounded by @c maxCount and protected by a FastMutex;
 *  - the "new processes" list used for remote-thread detection, bounded by
 *    @c processesMaxCount and protected by an ExecutiveResource.
 */
struct Globals
{
    /**
     * @brief Initializes the lists, locks and counters. Must be called once from DriverEntry.
     *
     * @param maxCount          Maximum number of pending event items
     * @param processesMaxCount Maximum number of tracked new processes
     */
    VOID Init(_In_ ULONG maxCount, _In_ ULONG processesMaxCount);

    /**
     * @brief Releases the locks. Call after both lists have been drained.
     */
    VOID Destroy();

    /**
     * @brief Appends an event item at the tail of the item list.
     *        When the list is full, the oldest item is dropped and freed.
     *
     * @param entry LIST_ENTRY of a FullItem<T>
     */
    VOID AddItem(_In_ LIST_ENTRY* entry);

    /**
     * @brief Inserts an event item back at the head of the item list
     *        (used when the client buffer is too small to consume it).
     *
     * @param entry LIST_ENTRY of a FullItem<T>
     */
    VOID AddHeadItem(_In_ LIST_ENTRY* entry);

    /**
     * @brief Pops the oldest event item from the item list.
     *
     * @return LIST_ENTRY of the removed item, or nullptr if the list is empty
     */
    LIST_ENTRY* RemoveItem();

    /**
     * @brief Registers a newly created process for remote-thread detection.
     *
     * @param entry LIST_ENTRY of a ProcessItem
     * @return true if inserted, false if the list is full
     */
    bool AddNewProcess(_In_ LIST_ENTRY* entry);

    /**
     * @brief Removes (and frees) the tracked process matching @p pid.
     *
     * @param pid Process ID to remove
     * @return true if the process was found and removed
     */
    bool RemoveProcess(_In_ HANDLE pid);

    /**
     * @brief Frees every entry of the new-processes list.
     */
    VOID ClearNewProcesses();

    /**
     * @brief
     * @param function
     * @param altitude
     * @param driver
     * @param context
     * @param reserved
     */
    NTSTATUS RegisterCallback(
        _In_       PEX_CALLBACK_FUNCTION function,
        _In_       PCUNICODE_STRING      altitude,
        _In_       PVOID                 driver,
        _In_opt_   PVOID                 context,
        _Reserved_ PVOID                 reserved
    );

    /**
     * 
     */
    LARGE_INTEGER GetCookie() CONST;

    /**
     * 
     */
    VOID UnRegisterCallback(); 

private:
    // Event items: Process, Thread, Image & RemoteThread
    LIST_ENTRY _ItemsHead;
    ULONG      _Count;
    ULONG      _MaxCount;
    FastMutex  _Lock;
    // New processes tracked for remote-thread detection
    LIST_ENTRY        _NewProcessesHead;
    ULONG             _NewProcessesCount;
    ULONG             _NewProcessesMaxCount;
    ExecutiveResource _NewProcessesLock;
    // 
    LARGE_INTEGER     _Cookie;
};
