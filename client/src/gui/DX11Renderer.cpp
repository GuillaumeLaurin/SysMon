#include "gui/DX11Renderer.hpp"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

bool DX11Renderer::Initialize(HWND hwnd)
{
  DXGI_SWAP_CHAIN_DESC sd = {};
  sd.BufferCount = 2;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hwnd;
  sd.SampleDesc.Count = 1;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  D3D_FEATURE_LEVEL featureLevel;
  constexpr D3D_FEATURE_LEVEL levels[] = {
      D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
  
  HRESULT res = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
      levels, 2, D3D11_SDK_VERSION,
      &sd, &_SwapChain, &_Device, &featureLevel,
      &_DeviceContext);
  
  if (res == DXGI_ERROR_UNSUPPORTED) 
  {
      res = D3D11CreateDeviceAndSwapChain(
          nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0,
          levels, 2, D3D11_SDK_VERSION,
          &sd, &_SwapChain, &_Device, &featureLevel,
          &_DeviceContext);
  }

  if (res != S_OK) 
  {
      return false;
  }

  if (!CreateRenderTarget()) 
  {
      return false;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(_Device, _DeviceContext);

  return true;
}

void DX11Renderer::BeginFrame()
{
    if (_ResizeWidth != 0 && _ResizeHeight != 0) 
    {
        CleanupRenderTarget();
        _SwapChain->ResizeBuffers(0, _ResizeWidth, _ResizeHeight, 
            DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
        _ResizeWidth = _ResizeHeight = 0;
        CreateRenderTarget();
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void DX11Renderer::EndFrame()
{
    ImGui::Render();

    _DeviceContext->OMSetRenderTargets(1, &_RenderTarget, nullptr);
    _DeviceContext->ClearRenderTargetView(_RenderTarget, _ClearColor);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    _SwapChain->Present(1, 0);
}

void DX11Renderer::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupRenderTarget();
    _SwapChain->Release();
    _DeviceContext->Release();
    _Device->Release();
}

void DX11Renderer::SetResizeSize(UINT width, UINT height)
{
    _ResizeWidth = width;
    _ResizeHeight = height;
}

bool DX11Renderer::CreateRenderTarget()
{
    ID3D11Texture2D* backBuffer = nullptr;
    _SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    HRESULT res = _Device->CreateRenderTargetView(backBuffer, nullptr, &_RenderTarget);
    backBuffer->Release();
    return res == S_OK;
}

void DX11Renderer::CleanupRenderTarget()
{
    if (_RenderTarget) 
    {
        _RenderTarget->Release();
        _RenderTarget = nullptr;
    }
}