#pragma once

#include "interfaces/IPageManager.hpp"

#include <string>
#include <memory>

#include <unordered_map>

class PageManager : public IPageManager
{
public:
    PageManager() = default;
    ~PageManager() override = default;

    void RegisterPage(const std::string& id, std::shared_ptr<IPage> page) override;
    void ActivatePage(const std::string& id) override;
    std::shared_ptr<IPage> GetPage(const std::string& id) const override;
    std::shared_ptr<IPage> CurrentPage() const override;

private:
    std::unordered_map<std::string, std::shared_ptr<IPage>> _Pages;
    std::shared_ptr<IPage> _CurrentPage;
};
