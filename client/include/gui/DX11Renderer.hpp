#pragma once

#include "interfaces/IRenderer.hpp"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

class DX11Renderer : public IRenderer
{
public:
    ~DX11Renderer() override = default;

    bool Initialize(HWND hwnd) override;
    void BeginFrame() override;
    void EndFrame() override;
    void Shutdown() override;

    void SetResizeSize(UINT width, UINT height) override;

private:
    ID3D11Device*           _Device         = nullptr;
    ID3D11DeviceContext*    _DeviceContext  = nullptr;
    IDXGISwapChain*         _SwapChain      = nullptr;
    ID3D11RenderTargetView* _RenderTarget   = nullptr;

    UINT _ResizeWidth = 0;
    UINT _ResizeHeight = 0;
    bool _SwapChainOccluded = false;
  
    const float _ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

    bool CreateRenderTarget();
    void CleanupRenderTarget();
};