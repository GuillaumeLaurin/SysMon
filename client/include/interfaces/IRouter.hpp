#pragma once

#include <memory>
#include <string>

#include "interfaces/IPage.hpp"

class IRouter
{
public:
    virtual ~IRouter() = default;

    virtual void Navigate(const std::string& pageId) = 0;
    virtual bool GoBack() = 0;
    virtual std::shared_ptr<IPage> CurrentPage() const = 0;
};