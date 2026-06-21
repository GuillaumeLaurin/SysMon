#pragma once

#include "interfaces/IEventRepository.hpp"

#include "interfaces/IDatabase.hpp"

#include <memory>

class EventRepository : public IEventRepository
{
public:
    EventRepository(std::shared_ptr<IDatabase> connexion);

    ~EventRepository() override = default;

    bool Insert(const EventRecord& record) noexcept override;
    std::vector<EventRecord> QueryAll() noexcept override;
    std::vector<EventRecord> QueryByType(std::string_view type) noexcept override;
    std::vector<EventRecord> QueryByPid(ULONG pid) noexcept override;
    bool Clear() noexcept override;

private:
    std::shared_ptr<IDatabase> _Connexion;
};