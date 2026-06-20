#pragma once

#include "interfaces/IConfigRepository.hpp"

#include "interfaces/IDatabase.hpp"

#include <memory>

class ConfigRepository : public IConfigRepository
{
public:
    ConfigRepository(std::shared_ptr<IDatabase> connexion);

    ~ConfigRepository() override = default;

    bool Set(std::string_view key, std::string_view value) noexcept override;
    std::string Get(std::string_view key, std::string_view defaultValue = "") noexcept override;
    bool Remove(std::string_view key) noexcept override;
    bool Exists(std::string_view key) const noexcept override;

private:
    std::shared_ptr<IDatabase> _Connexion;
};