#include "Common.h"

#include "Globals.h"

#include "Public.h"

#include "Locker.h"

VOID Globals::Init(_In_ ULONG maxCount)
{
    InitializeListHead(&_ItemsHead);
    _Lock.Init();
    _Count = 0;
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

Globals g_State;