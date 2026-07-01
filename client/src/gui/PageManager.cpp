#include "gui/PageManager.hpp"

#include "exceptions/SysMonException.hpp"

void PageManager::RegisterPage(const std::string& id, std::shared_ptr<IPage> page)
{
    _Pages[id] = std::move(page);
}

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

std::shared_ptr<IPage> PageManager::GetPage(const std::string& id) const
{
  auto it = _Pages.find(id);
  return it != _Pages.end() ? it->second : nullptr;
}

std::shared_ptr<IPage> PageManager::CurrentPage() const
{
  return _CurrentPage;
}