/**
 * @file IRouter.hpp
 * @brief Contract for page navigation with history on top of the page manager.
 */

#pragma once

#include <memory>
#include <string>

#include "interfaces/IPage.hpp"

/**
 * @brief Page navigation with history (back stack) on top of the page manager.
 */
class IRouter
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IRouter() = default;

    /**
     * @brief Navigates to the page with the given identifier, pushing the current page on the history.
     * @param pageId Identifier of the destination page, as registered in the page manager.
     */
    virtual void Navigate(const std::string& pageId) = 0;

    /**
     * @brief Returns to the previous page; false when the history is empty.
     * @return true if navigation to the previous page succeeded, false when the history is empty.
     */
    virtual bool GoBack() = 0;

    /**
     * @brief Returns the currently displayed page.
     * @return Shared pointer to the currently displayed page, or nullptr if none is active yet.
     */
    virtual std::shared_ptr<IPage> CurrentPage() const = 0;
};
