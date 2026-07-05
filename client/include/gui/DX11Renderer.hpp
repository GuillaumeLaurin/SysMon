#pragma once

#include "interfaces/IRenderer.hpp"

/**
 * @file DX11Renderer.hpp
 * @brief IRenderer implementation backed by DirectX 11 and the ImGui Win32/DX11 backends.
 */

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

/**
 * @brief IRenderer implementation on DirectX 11 hosting the ImGui backends.
 *
 * @note Owns raw COM pointers (device, device context, swap chain, render target
 *       view) and is responsible for releasing them; call Shutdown() before
 *       destruction to avoid leaking D3D11/ImGui resources.
 */
class DX11Renderer : public IRenderer
{
public:
    /** @brief Default destructor; does not release D3D11 resources (call Shutdown() explicitly). */
    ~DX11Renderer() override = default;

    /**
     * @brief Creates the D3D11 device/swap chain and initializes the ImGui Win32/DX11 backends.
     * @param hwnd Native window handle the swap chain renders into.
     * @return true on success, false if device/swap-chain creation or render-target creation failed.
     */
    bool Initialize(HWND hwnd) override;

    /**
     * @brief Handles pending resize/occlusion, then starts a new ImGui frame.
     */
    void BeginFrame() override;

    /**
     * @brief Renders the ImGui draw data and presents the swap chain.
     */
    void EndFrame() override;

    /**
     * @brief Shuts down ImGui and releases every D3D11 resource.
     * @warning Must be called before the object is destroyed, otherwise the
     *          D3D11 device, device context, swap chain and render target view
     *          are leaked.
     */
    void Shutdown() override;

    /**
     * @brief Records a pending swap-chain resize applied at the next BeginFrame().
     * @param width New client-area width, in pixels.
     * @param height New client-area height, in pixels.
     */
    void SetResizeSize(UINT width, UINT height) override;

private:
    /** @brief D3D11 device used to create resources (render targets, etc.). */
    ID3D11Device*           _Device         = nullptr;

    /** @brief Immediate device context used to issue rendering commands. */
    ID3D11DeviceContext*    _DeviceContext  = nullptr;

    /** @brief Swap chain owning the back buffers presented to the window. */
    IDXGISwapChain*         _SwapChain      = nullptr;

    /** @brief Render-target view bound to the current swap-chain back buffer. */
    ID3D11RenderTargetView* _RenderTarget   = nullptr;

    /** @brief Pending resize width (0 when no resize is pending), applied at the next BeginFrame(). */
    UINT _ResizeWidth = 0;

    /** @brief Pending resize height (0 when no resize is pending), applied at the next BeginFrame(). */
    UINT _ResizeHeight = 0;

    /** @brief True when the swap chain reported occlusion (currently unused by the resize logic). */
    bool _SwapChainOccluded = false;

    /** @brief RGBA color used to clear the render target at the start of each frame. */
    const float _ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

    /**
     * @brief Creates the render-target view bound to the swap-chain back buffer.
     * @return true if the render-target view was created successfully.
     */
    bool CreateRenderTarget();

    /**
     * @brief Releases the render-target view.
     */
    void CleanupRenderTarget();
};
