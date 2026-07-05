#include "gui/PageManager.hpp"

#include "exceptions/SysMonException.hpp"

/** @brief Stores (or replaces) the page under the given id in the internal map. */
void PageManager::RegisterPage(const std::string& id, std::shared_ptr<IPage> page)
{
    _Pages[id] = std::move(page);
}

/**
 * @brief Looks up the page by id, calls OnExit() on the current page (if any),
 *        then makes the found page current and calls its OnEnter().
 * @throws SysMonException (via THROW_SYSMON) if @p id is not a registered page.
 */
void PageManager::ActivatePage(const std::string& id)
{
    auto it = _Pages.find(id);

    if (it == _Pages.end())
        THROW_SYSMON("Unknown page id: " + id, "GUI", "PageManager", "ActivatePage");

    if (_CurrentPage)
        _CurrentPage->OnExit();

    _CurrentPage = it->second;
    _CurrentPage->OnEnter();
}

/** @brief Looks up a registered page by id without changing the active page. */
std::shared_ptr<IPage> PageManager::GetPage(const std::string& id) const
{
  auto it = _Pages.find(id);
  return it != _Pages.end() ? it->second : nullptr;
}

/** @brief Returns the page previously set by ActivatePage(), if any. */
std::shared_ptr<IPage> PageManager::CurrentPage() const
{
  return _CurrentPage;
}