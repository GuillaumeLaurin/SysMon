#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

class IWindow
{
public:
    virtual ~IWindow() = default;

    virtual bool Initialize(HINSTANCE hInstance, int nCmdShow) = 0;
    virtual void ProcessMessages() = 0;
    virtual void Shutdown() = 0;

    virtual HWND GetHWND() const = 0;
    virtual bool IsRunning() const = 0;
};