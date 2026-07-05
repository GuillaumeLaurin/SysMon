/**
 * @file IRenderer.hpp
 * @brief Contract for the graphics backend abstraction driving ImGui frames.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

/**
 * @brief Graphics backend abstraction driving ImGui frames (DirectX 11 implementation).
 *
 * @note Expected call order per frame is BeginFrame() ... EndFrame(), after a
 *       successful Initialize() and before Shutdown().
 */
class IRenderer
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     * @note Implementations should call Shutdown() if still initialized.
     */
    virtual ~IRenderer() = default;

    /**
     * @brief Creates the device/swap chain and binds ImGui to @p hwnd.
     * @param hwnd Native window handle to bind the swap chain to.
     * @return true on success, false if the graphics device could not be created.
     */
    virtual bool Initialize(HWND hwnd) = 0;

    /**
     * @brief Starts a new ImGui frame.
     */
    virtual void BeginFrame() = 0;

    /**
     * @brief Renders the ImGui draw data and presents the frame.
     */
    virtual void EndFrame() = 0;

    /**
     * @brief Releases the graphics resources and shuts ImGui down.
     */
    virtual void Shutdown() = 0;

    /**
     * @brief Records a pending swap-chain resize applied on the next frame.
     * @param width New back-buffer width, in pixels.
     * @param height New back-buffer height, in pixels.
     */
    virtual void SetResizeSize(UINT width, UINT height) = 0;
};
