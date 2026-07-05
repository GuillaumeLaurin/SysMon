/**
 * @file EventTablePage.hpp
 * @brief Reusable page base combining a filter bar and an auto-refreshed event table.
 */

#pragma once

#include "interfaces/IPage.hpp"
#include "interfaces/IEventRepository.hpp"
#include "interfaces/IConfigRepository.hpp"

#include "gui/components/EventTable.hpp"
#include "gui/components/FilterBar.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Reusable page base combining a filter bar and an event table,
 *        auto-refreshed from the repository. Processes/Threads/Images derive from it.
 */
class EventTablePage : public IPage
{
public:
    /**
     * @param eventRepository  Source of the displayed events
     * @param configRepository Provides refresh interval and max row count
     * @param types            Event types this page displays and filters on
     * @param dataHeader       Label of the table's data column
     */
    EventTablePage(
        std::shared_ptr<IEventRepository>  eventRepository,
        std::shared_ptr<IConfigRepository> configRepository,
        std::vector<std::string>           types,
        const std::string&                 dataHeader);
    /**
     * @brief Default destructor.
     */
    ~EventTablePage() override = default;

    /**
     * @brief Reloads the settings and forces a records refresh.
     */
    void OnEnter() override;

    /**
     * @brief Clears the displayed records when the page becomes inactive.
     */
    void OnExit() override;

    /**
     * @brief Periodically refreshes the records from the repository.
     */
    void Update() override;

    /**
     * @brief Draws the filter bar and the filtered event table.
     * @note Wraps its content in an ImGui::Begin()/End() window pair.
     */
    void Render() override;

    /**
     * @brief Base implementation; always false. Derived pages may override it.
     * @return False.
     */
    bool HasBadge() const override;

protected:
    /** Unfiltered records currently loaded from the repository (capped at _MaxRows). */
    std::vector<EventRecord> _Records;
    /** Table component used to render the filtered records. */
    EventTable               _Table;
    /** Filter bar component used to narrow down the displayed records. */
    FilterBar                _FilterBar;

private:
    /** Source of the displayed events. */
    std::shared_ptr<IEventRepository>     _EventRepository;
    /** Provides the refresh interval and max row count settings. */
    std::shared_ptr<IConfigRepository>    _ConfigRepository;
    /** Event types this page displays and filters on. */
    std::vector<std::string>              _Types;
    /** Minimum delay between two automatic record refreshes. */
    std::chrono::milliseconds             _RefreshInterval;
    /** Maximum number of rows kept in _Records. */
    std::size_t                           _MaxRows;
    /** Timestamp of the last record refresh, used to throttle Update(). */
    std::chrono::steady_clock::time_point _LastRefresh;

    /**
     * @brief Reloads the records of the configured types, capped at the max row count.
     */
    void RefreshRecords();
};
