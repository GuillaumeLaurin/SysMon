#include "Common.h"
#include "ExecutiveResource.h"

void ExecutiveResource::Init()
{
    ExInitializeResourceLite(&_Res);
}

void ExecutiveResource::Delete()
{
    ExDeleteResourceLite(&_Res);
}

void ExecutiveResource::Lock()
{
    _CritRegion = KeAreApcsDisabled();
    if (_CritRegion)
    {
        ExAcquireResourceExclusiveLite(&_Res, TRUE);
    }
    else
    {
        ExEnterCriticalRegionAndAcquireResourceExclusive(&_Res);
    }
}

void ExecutiveResource::Unlock()
{
    if (_CritRegion)
    {
        ExReleaseResourceLite(&_Res);
    }
    else
    {
        ExReleaseResourceAndLeaveCriticalRegion(&_Res);
    }
}

void ExecutiveResource::LockShared()
{
    _CritRegion = KeAreApcsDisabled();
    if (_CritRegion)
    {
        ExAcquireResourceSharedLite(&_Res, TRUE);
    }
    else
    {
        ExEnterCriticalRegionAndAcquireResourceShared(&_Res);
    }
}

void ExecutiveResource::UnlockShared()
{
    Unlock();
}