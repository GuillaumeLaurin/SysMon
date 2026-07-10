#include "Common.h"

#include "Globals.h"

#include "Public.h"

#include "Locker.h"

/**
 * @file Globals.cpp
 * @brief Implements Globals, declared in Globals.h, and defines the single
 *        driver-wide g_State instance.
 */

/**
 * @brief Initializes the lists, locks and counters.
 */
VOID Globals::Init(_In_ ULONG maxCount, _In_ ULONG processesMaxCount)
{
    InitializeListHead(&_ItemsHead);
    InitializeListHead(&_NewProcessesHead);
    _Lock.Init();
    _NewProcessesLock.Init();
    _Count = 0;
    _NewProcessesCount = 0;
    _MaxCount = maxCount;
    _NewProcessesMaxCount = processesMaxCount;
}

/**
 * @brief Releases the locks.
 *
 * @note Assumes both lists have already been drained by the caller.
 */
VOID Globals::Destroy()
{
    _Lock.Destroy();
    _NewProcessesLock.Delete();
}

/**
 * @brief Appends an event item at the tail of the item list, evicting the
 *        oldest entry first if the list is already full.
 */
VOID Globals::AddItem(_In_ LIST_ENTRY* entry)
{
    Locker locker(_Lock);

    if (_Count == _MaxCount)
    {
        auto head = RemoveHeadList(&_ItemsHead);
        auto item = CONTAINING_RECORD(head, FullItem<ItemHeader>, Entry);
        SAFE_FREE(item);
        _Count--;
    }

    InsertTailList(&_ItemsHead, entry);
    _Count++;
}

/**
 * @brief Inserts an event item back at the head of the item list.
 */
VOID Globals::AddHeadItem(_In_ LIST_ENTRY* entry)
{
    Locker locker(_Lock);
    InsertHeadList(&_ItemsHead, entry);
    _Count++;
}

/**
 * @brief Pops the oldest event item from the item list.
 */
LIST_ENTRY* Globals::RemoveItem()
{
    Locker locker(_Lock);

    auto item = RemoveHeadList(&_ItemsHead);

    if (item == &_ItemsHead)
    {
        return nullptr;
    }

    _Count--;
    return item;
}

/**
 * @brief Registers a newly created process for remote-thread detection.
 */
bool Globals::AddNewProcess(_In_ LIST_ENTRY* entry)
{
    Locker locker(_NewProcessesLock);

    if (_NewProcessesCount == _NewProcessesMaxCount)
    {
        return false;
    }

    InsertTailList(&_NewProcessesHead, entry);
    _NewProcessesCount++;
    return true;
}

/**
 * @brief Removes (and frees) the tracked process matching @p pid.
 *
 * @note Performs a linear scan of the new-processes list under the lock.
 */
bool Globals::RemoveProcess(_In_ HANDLE pid)
{
    auto id = HandleToULong(pid);
    Locker locker(_NewProcessesLock);

    for (auto entry = _NewProcessesHead.Flink; entry != &_NewProcessesHead; entry = entry->Flink)
    {
        auto item = CONTAINING_RECORD(entry, ProcessItem, Entry);

        if (item->Data.ProcessId == id)
        {
            RemoveEntryList(&item->Entry);
            SAFE_FREE(item);
            _NewProcessesCount--;
            return true;
        }
    }

    return false;
}

/**
 * @brief Frees every entry of the new-processes list.
 */
VOID Globals::ClearNewProcesses()
{
    Locker locker(_NewProcessesLock);

    while (!IsListEmpty(&_NewProcessesHead))
    {
        auto entry = RemoveHeadList(&_NewProcessesHead);
        auto item = CONTAINING_RECORD(entry, ProcessItem, Entry);
        SAFE_FREE(item);
        _NewProcessesCount--;
    }
}

/**
 * 
 */
NTSTATUS Globals::RegisterCallback(
    _In_       PEX_CALLBACK_FUNCTION function,
    _In_       PCUNICODE_STRING      altitude,
    _In_       PVOID                 driver,
    _In_opt_   PVOID                 context,
    _Reserved_ PVOID                 reserved 
)
{
    return CmRegisterCallbackEx(
        function,
        altitude,
        driver,
        context,
        &_Cookie,
        reserved
    );
}

/**
 * 
 */
LARGE_INTEGER Globals::GetCookie() CONST
{
    return _Cookie;
}

/**
 * 
 */
VOID Globals::UnRegisterCallback()
{
    CmUnRegisterCallback(_Cookie);
}

/**
 * @brief The single global instance holding all driver state.
 */
Globals g_State;