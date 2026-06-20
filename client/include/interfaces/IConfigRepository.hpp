#pragma once

#include <string>

class IConfigRepository
{
public:
    virtual ~IConfigRepository() = default;

    virtual bool Set(std::string_view key, std::string_view value) noexcept = 0;
    virtual std::string Get(std::string_view key, std::string_view defaultValue = "") noexcept = 0;
    virtual bool Remove(std::string_view key) noexcept = 0;
    virtual bool Exists(std::string_view key) const noexcept = 0;
};