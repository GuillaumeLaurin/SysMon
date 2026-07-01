#pragma once

#include "interfaces/IRouter.hpp"
#include "interfaces/IPageManager.hpp"

#include <vector>
#include <string>
#include <memory>

class Router : public IRouter
{
public:
    explicit Router(std::shared_ptr<IPageManager> pageManager);
    ~Router() override = default;

    void Navigate(const std::string& pageId) override;
    bool GoBack() override;
    std::shared_ptr<IPage> CurrentPage() const override;

private:
    std::shared_ptr<IPageManager> _PageManager;
    std::vector<std::string>      _History;
};