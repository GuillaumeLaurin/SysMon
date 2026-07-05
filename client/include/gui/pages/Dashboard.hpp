/**
 * @file Dashboard.hpp
 * @brief Home page showing per-event-type counters and a remote-thread alert banner.
 */

#pragma once

#include "interfaces/IPage.hpp"

#include "interfaces/IEventRepository.hpp"

#include "gui/components/StatCard.hpp"
#include "gui/components/AlertBanner.hpp"

#include "macros/SysMonMacros.hpp"

#include <memory>
#include <string>
#include <chrono>
#include <vector>

#include <unordered_map>

/**
 * @brief Home page showing per-event-type counters as stat cards
 *        plus an alert banner for remote-thread detections.
 */
class Dashboard : public IPage
{
    _PAGE_NAME "Dashboard";
    SYSMON_CLASS(Dashboard);
public:
    /**
     * @brief Constructs the dashboard and its stat cards for every known event type.
     * @param eventRepository Source of the events used to compute the counters.
     */
    Dashboard(std::shared_ptr<IEventRepository> eventRepository);

    /**
     * @brief Default destructor.
     */
    ~Dashboard() override = default;

    /**
     * @brief Forces a cache refresh when the page becomes active.
     */
    void OnEnter() override;

    /**
     * @brief Clears the cached per-type counters when the page becomes inactive.
     */
    void OnExit() override;

    /**
     * @brief Periodically refreshes the counters from the repository.
     */
    void Update() override;

    /**
     * @brief Draws the stat cards and the alert banner.
     * @note Wraps its content in an ImGui::Begin()/End() window pair.
     */
    void Render() override;

    /**
     * @brief Human-readable title shown in the sidebar/header.
     * @return The page's static name.
     */
    const char* GetTitle() const override;

    /**
     * @brief Returns true when a RemoteThread event has been recorded.
     * @return True if the sidebar entry should display a badge.
     */
    bool        HasBadge() const override;

private:
    /** Source of the events used to compute the per-type counters. */
    std::shared_ptr<IEventRepository>            _EventRepository;
    /** Cached per-event-type counters, keyed by event type name. */
    std::unordered_map<std::string, std::size_t> _Cache;
    /** Timestamp of the last cache refresh, used to throttle Update(). */
    std::chrono::steady_clock::time_point        _LastRefresh;

    /** One stat card per known event type. */
    std::vector<StatCard> _Cards;
    /** Banner used to surface RemoteThread detections. */
    AlertBanner           _Banner;

    /**
     * @brief Recomputes the per-type counters from the stored events.
     */
    void RefreshCache();
};
