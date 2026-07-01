#pragma once

#include <memory>
#include <string>

#include "interfaces/IPage.hpp"

class IPageManager
{
public:
    virtual ~IPageManager() = default;

    virtual void RegisterPage(const std::string& id, std::shared_ptr<IPage> page) = 0;
    virtual void ActivatePage(const std::string& id) = 0;
    virtual std::shared_ptr<IPage> GetPage(const std::string& id) const = 0;
    virtual std::shared_ptr<IPage> CurrentPage() const = 0;
};