#include "Common.h"

#include "Globals.h"

#include "Public.h"

#include "Locker.h"

#define DEFAULT_COUNT 0

VOID Globals::Init(_In_ ULONG maxCount)
{
    InitializeListHead(&_ItemsHead);
    _Lock.Init();
    _Count = DEFAULT_COUNT;
    _MaxCount = maxCount;
}

VOID Globals::Destroy()
{
    _Lock.Destroy();
}

VOID Globals::AddItem(_In_ LIST_ENTRY* entry)
{
    Locker locker(_Lock);

    if (_Count == _MaxCount)
    {
        auto head = RemoveHeadList(&_ItemsHead);
        ExFreePool(CONTAINING_RECORD(head, FullItem, Entry));
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

Globals g_State;