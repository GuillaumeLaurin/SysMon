/**
 * @file Sidebar.hpp
 * @brief Navigation sidebar listing the registered pages.
 */

#pragma once

#include "interfaces/IComponent.hpp"
#include "interfaces/IRouter.hpp"
#include "interfaces/IPageManager.hpp"

#include <memory>
#include <string>
#include <vector>

/**
 * @brief Navigation sidebar listing the registered pages; clicking an entry
 *        navigates through the router.
 */
class Sidebar : public IComponent
{
public:
    /**
     * @brief Constructs the sidebar.
     * @param router Used to read the current page and to trigger navigation.
     * @param pageManager Used to resolve page identifiers into IPage instances.
     */
    Sidebar(std::shared_ptr<IRouter> router, std::shared_ptr<IPageManager> pageManager);

    /**
     * @brief Default destructor.
     */
    ~Sidebar() override = default;

    /**
     * @brief Appends a navigation entry for the page with the given identifier.
     * @param pageId Identifier of the page to add, as resolved by the page manager.
     */
    void AddEntry(const std::string& pageId);

    /**
     * @brief Updates the component state; called once per frame before Render().
     */
    void Update() override;

    /**
     * @brief Draws the sidebar and handles navigation clicks.
     * @note Draws its own fixed, borderless ImGui window docked to the left
     *       edge of the viewport; it does not need to be wrapped in Begin/End.
     */
    void Render() override;

    /**
     * @brief Sidebar width in pixels, used to lay out the page area.
     * @return Fixed sidebar width in pixels.
     */
    float Width() const;

private:
    /** Router used to read the active page and trigger navigation. */
    std::shared_ptr<IRouter>      _Router;
    /** Page manager used to resolve entries into IPage instances. */
    std::shared_ptr<IPageManager> _PageManager;
    /** Ordered list of registered page identifiers shown in the sidebar. */
    std::vector<std::string>      _Entries;
};
