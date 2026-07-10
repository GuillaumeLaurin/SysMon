#include "gui/pages/Dashboard.hpp"

#include <imgui.h>

#include <string_view>
#include <cstddef>

constexpr const char* KNOWN_TYPES[] = {
    "ProcessCreate", "ProcessExit", "ThreadCreate", "ThreadExit", "ImageLoad", "RemoteThread",
    "RegistrySetValue"
};

constexpr auto REFRESH_INTERVAL = std::chrono::milliseconds(500);

namespace
{
    ImVec4 ColorForType(std::string_view type)
    {
        if (type == "RemoteThread")
            return ImVec4(0.94f, 0.32f, 0.29f, 1.0f);

        if (type == "ImageLoad")
            return ImVec4(0.55f, 0.49f, 0.96f, 1.0f);

        if (type == "RegistrySetValue")
            return ImVec4(0.96f, 0.62f, 0.26f, 1.0f);

        return ImVec4(0.25f, 0.80f, 0.49f, 1.0f);
    }
}

/** @brief Constructs the dashboard and its stat cards for every known event type. */
Dashboard::Dashboard(std::shared_ptr<IEventRepository> eventRepository)
    : _EventRepository(eventRepository)
{
    for (const char* type : KNOWN_TYPES)
        _Cards.emplace_back(type, ColorForType(type));
}

/** @brief Forces a cache refresh when the page becomes active. */
void Dashboard::OnEnter()
{
    RefreshCache();
}

/** @brief Clears the cached per-type counters when the page becomes inactive. */
void Dashboard::OnExit()
{
    _Cache.clear();
}

/** @brief Periodically refreshes the counters from the repository. */
void Dashboard::Update()
{
    auto now = std::chrono::steady_clock::now();

    if ((now - _LastRefresh) < REFRESH_INTERVAL)
        return;

    RefreshCache();
    _LastRefresh = now;
}

/** @brief Draws the stat cards and the alert banner. */
void Dashboard::Render()
{
    ImGui::Begin(GetTitle());

    _Banner.Update();
    _Banner.Render();

    for (std::size_t i = 0; i < _Cards.size(); ++i)
    {
        _Cards[i].Update();
        _Cards[i].Render();

        if (i + 1 < _Cards.size())
            ImGui::SameLine();
    }

    ImGui::End();
}

/** @brief Human-readable title shown in the sidebar/header. */
const char* Dashboard::GetTitle() const
{
    return Name;
}

/** @brief Returns true when a RemoteThread event has been recorded. */
bool Dashboard::HasBadge() const
{
    auto it = _Cache.find("RemoteThread");
    return it != _Cache.end() && it->second > 0;
}

/** @brief Recomputes the per-type counters from the stored events. */
void Dashboard::RefreshCache()
{
    _Cache.clear();
    auto records = _EventRepository->QueryAll();

    for (auto& record : records)
    {
        _Cache[record.Type] += 1;
    }

    for (auto& card : _Cards)
    {
        auto it = _Cache.find(card.Label());
        card.SetValue(it != _Cache.end() ? it->second : 0);
    }

    auto remoteThreadCount = _Cache.count("RemoteThread") ? _Cache.at("RemoteThread") : 0;

    if (remoteThreadCount > 0)
        _Banner.SetMessage(std::to_string(remoteThreadCount) + " RemoteThread alert(s) detected");
    else
        _Banner.Clear();
}
