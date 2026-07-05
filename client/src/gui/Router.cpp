#include "gui/Router.hpp"

/** @brief Stores the page manager used to activate pages by id. */
Router::Router(std::shared_ptr<IPageManager> pageManager)
    : _PageManager(pageManager)
{
}

/**
 * @brief Appends the page id to the navigation history, then activates it.
 * @throws SysMonException Propagated from IPageManager::ActivatePage() if @p pageId is unknown.
 */
void Router::Navigate(const std::string& pageId)
{
    _History.push_back(pageId);
    _PageManager->ActivatePage(pageId);
}

/**
 * @brief Pops the current entry off the history and re-activates the previous one.
 * @note Requires at least 2 entries in the history to succeed (there must be
 *       a "previous" page to go back to).
 */
bool Router::GoBack()
{
    if (_History.size() <= 1)
        return false;

    _History.pop_back();
    _PageManager->ActivatePage(_History.back());
    return true;
}

/** @brief Delegates to the underlying page manager's CurrentPage(). */
std::shared_ptr<IPage> Router::CurrentPage() const
{
    return _PageManager->CurrentPage();
}