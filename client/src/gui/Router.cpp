#include "gui/Router.hpp"

Router::Router(std::shared_ptr<IPageManager> pageManager)
    : _PageManager(pageManager)
{
}

void Router::Navigate(const std::string& pageId)
{
    _History.push_back(pageId);
    _PageManager->ActivatePage(pageId);
}

bool Router::GoBack()
{
    if (_History.size() <= 1)
        return false;
    
    _History.pop_back();
    _PageManager->ActivatePage(_History.back());
    return true;
}

std::shared_ptr<IPage> Router::CurrentPage() const
{
    return _PageManager->CurrentPage();
}