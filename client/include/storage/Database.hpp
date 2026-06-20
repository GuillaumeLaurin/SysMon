#pragma once

#include "interfaces/IDatabase.hpp"

class Database : public IDatabase
{
public:
    ~Database() override = default;

    bool Open(std::string_view path) noexcept override;
    void Close() noexcept override;
    bool IsOpen() const noexcept override;
    bool Migrate() noexcept override;
    sqlite3* Handle() const noexcept override;

private:
    sqlite3* _Connexion = nullptr;
};