#include "gui/Win32Window.hpp"

#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

bool Win32Window::Initialize(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"SysMonWindow";

    RegisterClassExW(&wc);

    _Handle = CreateWindowExW(
        0, L"SysMonWindow", L"SysMon",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr, nullptr, hInstance, nullptr);
    
    if (!_Handle)
        return false;
    
    SetWindowLongPtr(_Handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(_Handle, nCmdShow);
    UpdateWindow(_Handle);
    _IsRunning = true;
    
    return true;
}

void Win32Window::ProcessMessages()
{
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            _IsRunning = false;
            return;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Win32Window::Shutdown()
{
    DestroyWindow(_Handle);
    UnregisterClassW(L"SysMonWindow", nullptr);
}

HWND Win32Window::GetHWND() const
{
    return _Handle;
}

bool Win32Window::IsRunning() const
{
    return _IsRunning;
}

void Win32Window::SetResizeCallback(ResizeCallback callback)
{
    _OnResize = std::move(callback);
}

LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;
    
    auto* window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg)
    {
        case WM_SIZE:
        {
            if (window && wParam != SIZE_MINIMIZED && window->_OnResize)
                window->_OnResize(LOWORD(lParam), HIWORD(lParam));
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

