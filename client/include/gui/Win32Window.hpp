#pragma once

#include "interfaces/IWindow.hpp"

#include <functional>

/**
 * @file Win32Window.hpp
 * @brief IWindow implementation wrapping a native Win32 top-level window and
 *        forwarding messages to ImGui and to application resize callbacks.
 */

/** @brief Invoked when the client area is resized, with the new width and height. */
using ResizeCallback = std::function<void(UINT, UINT)>;

/**
 * @brief IWindow implementation wrapping a native Win32 top-level window.
 *
 * @note Not thread-safe; ProcessMessages() and the window procedure are
 *       expected to run on the thread that created the window.
 */
class Win32Window : public IWindow
{
public:
    /** @brief Default destructor; does not destroy the native window (call Shutdown() explicitly). */
    ~Win32Window() override = default;

    /**
     * @brief Registers the window class, creates the window and shows it.
     * @param hInstance Handle to the application instance, as passed to WinMain.
     * @param nCmdShow Initial show-window command, as passed to WinMain.
     * @return true on success, false if window-class registration or window creation failed.
     */
    bool Initialize(HINSTANCE hInstance, int nCmdShow) override;

    /**
     * @brief Pumps pending Win32 messages; call once per frame.
     * @note Sets IsRunning() to false when a WM_QUIT message is received.
     */
    void ProcessMessages() override;

    /**
     * @brief Destroys the window and unregisters the class.
     * @warning Must be called before destruction to release the native window handle.
     */
    void Shutdown() override;

    /**
     * @brief Native window handle.
     * @return The HWND of the managed window, or nullptr if not yet initialized.
     */
    HWND GetHWND() const override;

    /**
     * @brief Returns false once WM_QUIT/WM_DESTROY has been received.
     * @return true while the message loop should keep running.
     */
    bool IsRunning() const override;

    /**
     * @brief Registers the callback notified on WM_SIZE.
     * @param callback Callback invoked with the new client-area width and height.
     */
    void SetResizeCallback(ResizeCallback callback);

private:
    /**
     * @brief Static window procedure dispatching messages to the instance.
     * @param hwnd Handle of the window receiving the message.
     * @param msg Message identifier.
     * @param wParam Message-specific parameter.
     * @param lParam Message-specific parameter.
     * @return Result of message processing, as required by the Win32 window procedure contract.
     */
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /** @brief Handle of the managed native window. */
    HWND           _Handle     = nullptr;

    /** @brief Whether the message loop should keep running. */
    bool           _IsRunning  = false;

    /** @brief Callback invoked from WndProc() on WM_SIZE. */
    ResizeCallback _OnResize;
};
