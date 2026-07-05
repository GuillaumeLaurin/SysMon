#pragma once

#include "interfaces/IRenderer.hpp"

#include "interfaces/IPage.hpp"

#include "interfaces/IRouter.hpp"

#include "gui/components/Sidebar.hpp"

#include <vector>
#include <memory>

/**
 * @file UIRenderer.hpp
 * @brief Top-level frame composer combining the sidebar and the page currently
 *        selected by the router, on top of an IRenderer backend.
 */

/**
 * @brief Composes each UI frame: sidebar plus the page currently selected by the router.
 */
class UIRenderer
{
public:
    /**
     * @brief Constructs a UI renderer bound to the given graphics renderer and router.
     * @param renderer Graphics renderer used to begin/end each frame.
     * @param router Router providing the page to display on each frame.
     */
    explicit UIRenderer(std::shared_ptr<IRenderer> renderer, std::shared_ptr<IRouter> router);

    /** @brief Default destructor; does not call Shutdown() automatically. */
    ~UIRenderer() = default;

    /**
     * @brief Initializes the underlying graphics renderer for the given window.
     * @param hwnd Native window handle to render into.
     * @return true on success, false if the underlying renderer failed to initialize.
     */
    bool Initialize(HWND hwnd);

    /**
     * @brief Sets the sidebar component drawn on every frame.
     * @param sidebar Sidebar instance to render alongside the active page.
     */
    void SetSidebar(std::shared_ptr<Sidebar> sidebar);

    /**
     * @brief Draws one full frame (sidebar + active page).
     * @note Assumes the router's current page is non-null; called once per application loop iteration.
     */
    void Render();

    /**
     * @brief Shuts the graphics renderer down.
     * @warning Must be called before destruction to release the underlying D3D11/ImGui resources.
     */
    void Shutdown();

private:
  /** @brief Graphics renderer used to begin/end and present each frame. */
  std::shared_ptr<IRenderer>          _Renderer;

  /** @brief Router providing the currently active page. */
  std::shared_ptr<IRouter>            _Router;

  /** @brief Sidebar drawn alongside the active page, if set. */
  std::shared_ptr<Sidebar>            _Sidebar;
};
