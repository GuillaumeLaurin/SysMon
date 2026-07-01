#include "gui/UIRenderer.hpp"

UIRenderer::UIRenderer(std::shared_ptr<IRenderer> renderer, std::shared_ptr<IRouter> router)
    : _Renderer(renderer), _Router(router)
{
}

bool UIRenderer::Initialize(HWND hwnd) 
{
    return  _Renderer->Initialize(hwnd);
}

void UIRenderer::Render()
{
    _Renderer->BeginFrame();

    _Router->CurrentPage()->Update();
    _Router->CurrentPage()->Render();

    _Renderer->EndFrame();
}

void UIRenderer::Shutdown()
{
    _Router->CurrentPage()->OnExit();
    
    _Renderer->Shutdown();
}