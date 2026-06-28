#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "core/DIContainer.hpp"

#include "gui/Win32Window.hpp"

#include <memory>
#include <atomic>

class Application
{
public:
    Application(HINSTANCE hInstance, int nCmdShow);

    uint32_t Run() noexcept;
    void Shutdown() noexcept;

private:
    DIContainer                  _Container;
    std::atomic<bool>            _Running;
    uint32_t                     _Error;
    
    HINSTANCE _HInstance;
    int _NCmdShow;
    std::unique_ptr<Win32Window> _Window;

    void LogicLoop();
    void Init();
};