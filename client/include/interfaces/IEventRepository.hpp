#pragma once

#include "storage/EventRecord.hpp"

#include <vector>

class IEventRepository
{
public:
    virtual ~IEventRepository() = default;

    virtual bool Insert(const EventRecord& record) noexcept = 0;
    virtual std::vector<EventRecord> QueryAll() noexcept = 0;
    virtual std::vector<EventRecord> QueryByType(std::string_view type) noexcept = 0;
    virtual std::vector<EventRecord> QueryByPid(ULONG pid) noexcept = 0;
    virtual bool Clear() noexcept = 0;
};