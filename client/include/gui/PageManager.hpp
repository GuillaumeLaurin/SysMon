#pragma once

#include "interfaces/IPageManager.hpp"

#include <string>
#include <memory>

#include <unordered_map>

/**
 * @file PageManager.hpp
 * @brief IPageManager implementation storing registered pages in a hash map
 *        and firing lifecycle callbacks when the active page changes.
 */

/**
 * @brief IPageManager implementation storing pages in a hash map and
 *        firing OnEnter/OnExit on activation changes.
 */
class PageManager : public IPageManager
{
public:
    /** @brief Constructs an empty page manager with no registered pages. */
    PageManager() = default;

    /** @brief Default destructor. */
    ~PageManager() override = default;

    /**
     * @brief Registers a page under the given identifier.
     * @param id Unique identifier the page will be looked up and activated by.
     * @param page Page instance to register.
     */
    void RegisterPage(const std::string& id, std::shared_ptr<IPage> page) override;

    /**
     * @brief Deactivates the current page (OnExit) and activates the requested one (OnEnter).
     * @param id Identifier of the page to activate.
     * @throws SysMonException If no page is registered under @p id.
     */
    void ActivatePage(const std::string& id) override;

    /**
     * @brief Returns the page registered under the given identifier.
     * @param id Identifier of the page to look up.
     * @return The registered page, or nullptr if @p id is unknown.
     */
    std::shared_ptr<IPage> GetPage(const std::string& id) const override;

    /**
     * @brief Returns the currently active page.
     * @return The active page, or nullptr if no page has been activated yet.
     */
    std::shared_ptr<IPage> CurrentPage() const override;

private:
    /** @brief Registered pages, keyed by their identifier. */
    std::unordered_map<std::string, std::shared_ptr<IPage>> _Pages;

    /** @brief Page currently active (last one passed to ActivatePage()), if any. */
    std::shared_ptr<IPage> _CurrentPage;
};
