#include "gui/pages/EventTablePage.hpp"

#include "gui/ConfigKeys.hpp"

#include <imgui.h>

#include <algorithm>
#include <cstdlib>

namespace
{
    std::size_t ParseSize(const std::string& value, const char* fallback)
    {
        auto parsed = std::strtoul(value.c_str(), nullptr, 10);
        return parsed > 0 ? parsed : std::strtoul(fallback, nullptr, 10);
    }
}

/** @brief Constructs the page, initializing the table and filter bar for the given event types. */
EventTablePage::EventTablePage(
    std::shared_ptr<IEventRepository>  eventRepository,
    std::shared_ptr<IConfigRepository> configRepository,
    std::vector<std::string>           types,
    const std::string&                 dataHeader
)   : _Table(dataHeader + "##Table", dataHeader)
    , _FilterBar("FilterBar", types)
    , _EventRepository(eventRepository)
    , _ConfigRepository(configRepository)
    , _Types(std::move(types))
    , _RefreshInterval(500)
    , _MaxRows(1000)
{
}

/** @brief Reloads the settings and forces a records refresh. */
void EventTablePage::OnEnter()
{
    _RefreshInterval = std::chrono::milliseconds(ParseSize(
        _ConfigRepository->Get(ConfigKeys::RefreshIntervalMs, ConfigKeys::DefaultRefreshIntervalMs),
        ConfigKeys::DefaultRefreshIntervalMs));

    _MaxRows = ParseSize(
        _ConfigRepository->Get(ConfigKeys::MaxRows, ConfigKeys::DefaultMaxRows),
        ConfigKeys::DefaultMaxRows);

    RefreshRecords();
    _LastRefresh = std::chrono::steady_clock::now();
}

/** @brief Clears the displayed records when the page becomes inactive. */
void EventTablePage::OnExit()
{
    _Records.clear();
    _Table.SetEvents({});
}

/** @brief Periodically refreshes the records from the repository. */
void EventTablePage::Update()
{
    auto now = std::chrono::steady_clock::now();

    if ((now - _LastRefresh) < _RefreshInterval)
        return;

    RefreshRecords();
    _LastRefresh = now;
}

/** @brief Draws the filter bar and the filtered event table. */
void EventTablePage::Render()
{
    ImGui::Begin(GetTitle());

    _FilterBar.Update();
    _FilterBar.Render();

    std::vector<EventRecord> filtered;
    filtered.reserve(_Records.size());

    for (const auto& record : _Records)
    {
        if (_FilterBar.Matches(record))
            filtered.push_back(record);
    }

    ImGui::Text("%zu event(s)", filtered.size());

    _Table.SetEvents(std::move(filtered));
    _Table.Update();
    _Table.Render();

    ImGui::End();
}

/** @brief Base implementation; always false. Derived pages may override it. */
bool EventTablePage::HasBadge() const
{
    return false;
}

/** @brief Reloads the records of the configured types, capped at the max row count. */
void EventTablePage::RefreshRecords()
{
    _Records.clear();

    for (const auto& type : _Types)
    {
        auto records = _EventRepository->QueryByType(type);
        _Records.insert(_Records.end(),
            std::make_move_iterator(records.begin()),
            std::make_move_iterator(records.end()));
    }

    std::sort(_Records.begin(), _Records.end(),
        [](const EventRecord& a, const EventRecord& b) {
            return a.Timestamp.QuadPart > b.Timestamp.QuadPart;
        });

    if (_Records.size() > _MaxRows)
        _Records.resize(_MaxRows);
}
