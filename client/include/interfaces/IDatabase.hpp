#pragma once

#include <string>

#include <sqlite3.h>

class IDatabase
{
public:
    virtual ~IDatabase() = default;

    virtual bool Open(std::string_view path) noexcept = 0;
    virtual void Close() noexcept = 0;
    virtual bool IsOpen() const noexcept = 0;
    virtual bool Migrate() noexcept = 0;
    virtual sqlite3* Handle() const noexcept = 0;
};