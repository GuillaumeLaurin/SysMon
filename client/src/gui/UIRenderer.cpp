#include "gui/UIRenderer.hpp"

#include <imgui.h>

/** @brief Stores the graphics renderer and router used to compose each frame. */
UIRenderer::UIRenderer(std::shared_ptr<IRenderer> renderer, std::shared_ptr<IRouter> router)
    : _Renderer(renderer), _Router(router)
{
}

/** @brief Delegates to the underlying IRenderer::Initialize(). */
bool UIRenderer::Initialize(HWND hwnd)
{
    return  _Renderer->Initialize(hwnd);
}

/** @brief Replaces the sidebar drawn on every subsequent frame. */
void UIRenderer::SetSidebar(std::shared_ptr<Sidebar> sidebar)
{
    _Sidebar = std::move(sidebar);
}

/**
 * @brief Begins the frame, updates/renders the sidebar (if set) offsetting
 *        the page viewport by its width, then updates/renders the router's
 *        current page and ends the frame.
 * @warning Assumes the router's current page is non-null.
 */
void UIRenderer::Render()
{
    _Renderer->BeginFrame();

    float offset = 0.0f;

    if (_Sidebar)
    {
        _Sidebar->Update();
        _Sidebar->Render();
        offset = _Sidebar->Width();
    }

    const ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(offset, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - offset, io.DisplaySize.y), ImGuiCond_Always);

    _Router->CurrentPage()->Update();
    _Router->CurrentPage()->Render();

    _Renderer->EndFrame();
}

/** @brief Calls OnExit() on the current page, then shuts the graphics renderer down. */
void UIRenderer::Shutdown()
{
    _Router->CurrentPage()->OnExit();

    _Renderer->Shutdown();
}
