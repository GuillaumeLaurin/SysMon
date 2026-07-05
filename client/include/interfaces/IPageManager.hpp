/**
 * @file IPageManager.hpp
 * @brief Contract for the owner of the application's pages and active-page tracking.
 */

#pragma once

#include <memory>
#include <string>

#include "interfaces/IPage.hpp"

/**
 * @brief Owns the pages of the application and tracks the active one.
 */
class IPageManager
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IPageManager() = default;

    /**
     * @brief Registers a page under the given identifier.
     * @param id Unique identifier used to activate/retrieve the page later.
     * @param page Shared ownership of the page instance to register.
     */
    virtual void RegisterPage(const std::string& id, std::shared_ptr<IPage> page) = 0;

    /**
     * @brief Makes the page with the given identifier the active page.
     * @param id Identifier of the page to activate, as passed to RegisterPage().
     * @note Implementations should call IPage::OnExit() on the previous active page
     *       and IPage::OnEnter() on the newly activated one.
     */
    virtual void ActivatePage(const std::string& id) = 0;

    /**
     * @brief Returns the page registered under @p id, or nullptr if unknown.
     * @param id Identifier used at registration time.
     * @return Shared pointer to the page, or nullptr when no page is registered under @p id.
     */
    virtual std::shared_ptr<IPage> GetPage(const std::string& id) const = 0;

    /**
     * @brief Returns the currently active page.
     * @return Shared pointer to the active page, or nullptr if none is active yet.
     */
    virtual std::shared_ptr<IPage> CurrentPage() const = 0;
};
