#include "Common.h"

#include "Globals.h"

#include "Public.h"

#include "Locker.h"

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

VOID Globals::Destroy()
{
    _Lock.Destroy();
    _NewProcessesLock.Delete();
}

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

VOID Globals::AddHeadItem(_In_ LIST_ENTRY* entry)
{
    Locker locker(_Lock);
    InsertHeadList(&_ItemsHead, entry);
    _Count++;
}

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

Globals g_State;