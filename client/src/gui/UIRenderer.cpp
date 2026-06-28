#include "gui/UIRenderer.hpp"

UIRenderer::UIRenderer(std::shared_ptr<IRenderer> renderer)
    : _Renderer(renderer)
{
}

bool UIRenderer::Initialize(HWND hwnd) 
{
    return  _Renderer->Initialize(hwnd);
}

void UIRenderer::Render()
{
    _Renderer->BeginFrame();

    for (auto& page : _Pages)
    {
        page->Update();
        page->Render();
    }

    _Renderer->EndFrame();
}

void UIRenderer::Shutdown()
{
    for (auto& page : _Pages)
        page->OnExit();
    
    _Renderer->Shutdown();
}

void UIRenderer::AddPage(std::shared_ptr<IPage> page)
{
    page->OnEnter();
    _Pages.push_back(std::move(page));
}