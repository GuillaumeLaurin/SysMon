#include "Common.h"
#include "ExecutiveResource.h"

/**
 * @file ExecutiveResource.cpp
 * @brief Implements ExecutiveResource, declared in ExecutiveResource.h.
 */

/**
 * @brief Initializes the underlying ERESOURCE.
 */
void ExecutiveResource::Init()
{
    ExInitializeResourceLite(&_Res);
}

/**
 * @brief Deletes the underlying ERESOURCE.
 */
void ExecutiveResource::Delete()
{
    ExDeleteResourceLite(&_Res);
}

/**
 * @brief Acquires the resource exclusively.
 *
 * @note Enters a critical region first unless APCs are already disabled,
 *       remembering which path was taken so Unlock() can mirror it.
 */
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

/**
 * @brief Releases an exclusive acquisition, leaving the critical region if one was entered.
 */
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

/**
 * @brief Acquires the resource in shared mode.
 *
 * @note Same critical-region handling as Lock().
 */
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

/**
 * @brief Releases a shared acquisition.
 *
 * @note Delegates to Unlock() since both paths release the resource identically.
 */
void ExecutiveResource::UnlockShared()
{
    Unlock();
}