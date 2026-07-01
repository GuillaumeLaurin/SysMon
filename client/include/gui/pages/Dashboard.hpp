#pragma once

#include "interfaces/IPage.hpp"

#include "interfaces/IEventRepository.hpp"

#include "macros/SysMonMacros.hpp"

#include <memory>
#include <string>
#include <chrono>

#include <unordered_map>

class Dashboard : public IPage
{
    _PAGE_NAME "Dashboard";
    SYSMON_CLASS(Dashboard);
public:
    Dashboard(std::shared_ptr<IEventRepository> eventRepository);
    ~Dashboard() override = default;

    void OnEnter() override;
    void OnExit() override;
    void Update() override;
    void Render() override;

    const char* GetTitle() const override;
    bool        HasBadge() const override;

private:
    std::shared_ptr<IEventRepository>            _EventRepository;
    std::unordered_map<std::string, std::size_t> _Cache;
    std::chrono::steady_clock::time_point        _LastRefresh;

    void RefreshCache();
};