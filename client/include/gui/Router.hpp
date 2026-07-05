#pragma once

#include "interfaces/IRouter.hpp"
#include "interfaces/IPageManager.hpp"

#include <vector>
#include <string>
#include <memory>

/**
 * @file Router.hpp
 * @brief IRouter implementation keeping a navigation history on top of an IPageManager.
 */

/**
 * @brief IRouter implementation keeping a navigation history on top of the page manager.
 */
class Router : public IRouter
{
public:
    /**
     * @brief Constructs a router bound to the given page manager.
     * @param pageManager Page manager used to activate pages by identifier.
     */
    explicit Router(std::shared_ptr<IPageManager> pageManager);

    /** @brief Default destructor. */
    ~Router() override = default;

    /**
     * @brief Activates the requested page and records it in the history.
     * @param pageId Identifier of the page to navigate to.
     * @throws SysMonException If the underlying page manager has no page registered under @p pageId.
     */
    void Navigate(const std::string& pageId) override;

    /**
     * @brief Re-activates the previous page; false when the history is empty.
     * @return true if navigation back succeeded, false when the history has fewer than 2 entries.
     */
    bool GoBack() override;

    /**
     * @brief Returns the currently displayed page.
     * @return The active page as reported by the underlying page manager.
     */
    std::shared_ptr<IPage> CurrentPage() const override;

private:
    /** @brief Page manager used to activate pages by identifier. */
    std::shared_ptr<IPageManager> _PageManager;

    /** @brief Stack of visited page identifiers, most recent last. */
    std::vector<std::string>      _History;
};
