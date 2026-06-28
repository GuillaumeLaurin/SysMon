#pragma once

#include "interfaces/IWindow.hpp"

#include <functional>

using ResizeCallback = std::function<void(UINT, UINT)>;

class Win32Window : public IWindow
{
public:
    ~Win32Window() override = default;

    bool Initialize(HINSTANCE hInstance, int nCmdShow) override;
    void ProcessMessages() override;
    void Shutdown() override;

    HWND GetHWND() const override;
    bool IsRunning() const override;

    void SetResizeCallback(ResizeCallback callback);

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND           _Handle     = nullptr;
    bool           _IsRunning  = false;
    ResizeCallback _OnResize;
};