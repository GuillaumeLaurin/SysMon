#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

class IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual bool Initialize(HWND hwnd) = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Shutdown() = 0;

    virtual void SetResizeSize(UINT width, UINT height) = 0;
};