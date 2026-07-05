#include "gui/Win32Window.hpp"

#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

/**
 * @brief Registers the "SysMonWindow" window class, creates a top-level
 *        overlapped window sized 1280x720, stores `this` in GWLP_USERDATA
 *        for WndProc(), and shows/updates the window.
 */
bool Win32Window::Initialize(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)); // IDC_ARROW
    wc.lpszClassName = L"SysMonWindow";

    RegisterClassExW(&wc);

    _Handle = CreateWindowExW(
        0, L"SysMonWindow", L"SysMon",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr, nullptr, hInstance, nullptr);
    
    if (!_Handle)
        return false;
    
    SetWindowLongPtrW(_Handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(_Handle, nCmdShow);
    UpdateWindow(_Handle);
    _IsRunning = true;
    
    return true;
}

/** @brief Drains the message queue with PeekMessageW, stopping the loop on WM_QUIT. */
void Win32Window::ProcessMessages()
{
    MSG msg = {};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            _IsRunning = false;
            return;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

/** @brief Destroys the native window and unregisters the "SysMonWindow" class. */
void Win32Window::Shutdown()
{
    DestroyWindow(_Handle);
    UnregisterClassW(L"SysMonWindow", nullptr);
}

/** @brief Returns the stored native window handle. */
HWND Win32Window::GetHWND() const
{
    return _Handle;
}

/** @brief Returns the running flag, cleared by ProcessMessages() on WM_QUIT. */
bool Win32Window::IsRunning() const
{
    return _IsRunning;
}

/** @brief Stores the callback invoked from WndProc() on WM_SIZE. */
void Win32Window::SetResizeCallback(ResizeCallback callback)
{
    _OnResize = std::move(callback);
}

/**
 * @brief Forwards messages to the ImGui Win32 handler first, then resolves the
 *        owning instance from GWLP_USERDATA to dispatch WM_SIZE (to the resize
 *        callback) and WM_DESTROY (posting WM_QUIT); other messages fall
 *        through to DefWindowProcW.
 */
LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;
    
    auto* window = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

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
    
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

