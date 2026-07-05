/**
 * @file IWindow.hpp
 * @brief Contract for the native window abstraction hosting the renderer.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

/**
 * @brief Native window abstraction (Win32 implementation) hosting the renderer.
 */
class IWindow
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     * @note Implementations should call Shutdown() if still running.
     */
    virtual ~IWindow() = default;

    /**
     * @brief Registers the window class and creates/shows the window.
     * @param hInstance Handle to the current module instance.
     * @param nCmdShow Initial show-window command, as passed to WinMain.
     * @return true on success, false if the window could not be created.
     */
    virtual bool Initialize(HINSTANCE hInstance, int nCmdShow) = 0;

    /**
     * @brief Pumps pending Win32 messages; call once per frame.
     */
    virtual void ProcessMessages() = 0;

    /**
     * @brief Destroys the window and unregisters the class.
     */
    virtual void Shutdown() = 0;

    /**
     * @brief Native window handle.
     * @return The HWND of the underlying window, or nullptr if not initialized.
     */
    virtual HWND GetHWND() const = 0;

    /**
     * @brief Returns false once the window has been closed.
     * @return true while the window is still open and processing messages.
     */
    virtual bool IsRunning() const = 0;
};
