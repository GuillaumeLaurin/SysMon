#pragma once

#ifdef USE_KMDF
#include <wdf.h>
#else
#include <wdm.h>
#endif

struct ExecutiveResource
{
    void Init();
    void Delete();

    void Lock();
    void Unlock();

    void LockShared();
    void UnlockShared();

private:
    ERESOURCE _Res;
    bool      _CritRegion;
};