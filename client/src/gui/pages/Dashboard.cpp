#include "gui/pages/Dashboard.hpp"

#include <imgui.h>

#include <string_view>
#include <cstddef>

constexpr const char* KNOWN_TYPES[] = {
    "ProcessCreate", "ProcessExit", "ThreadCreate", "ThreadExit", "ImageLoad", "RemoteThread"
};

constexpr auto REFRESH_INTERVAL = std::chrono::milliseconds(500);

Dashboard::Dashboard(std::shared_ptr<IEventRepository> eventRepository)
    : _EventRepository(eventRepository)
{
}

void Dashboard::OnEnter()
{
    RefreshCache();
}

void Dashboard::OnExit()
{
    _Cache.clear();
}

void Dashboard::Update()
{
    auto now = std::chrono::steady_clock::now();

    if ((now - _LastRefresh) < REFRESH_INTERVAL)
        return;
    
    RefreshCache();
    _LastRefresh = now;
}

void Dashboard::Render()
{
    ImGui::Begin(GetTitle());

    auto remoteThreadCount = _Cache.count("RemoteThread") ? _Cache.at("RemoteThread") : 0;

    if (remoteThreadCount > 0)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.32f, 0.29f, 1.0f));
        ImGui::Text("%zu alerte(s) RemoteThread detectee(s)", remoteThreadCount);
        ImGui::PopStyleColor();
        ImGui::Separator();
    }

    for (int i = 0; i < static_cast<int>(std::size(KNOWN_TYPES)); ++i)
    {
        const char* type = KNOWN_TYPES[i];
        auto count = _Cache.count(type) ? _Cache.at(type) : 0;

        ImVec4 color;

        if (std::string_view(type) == "RemoteThread")
            color = ImVec4(0.94f, 0.32f, 0.29f, 1.0f);
        else if (std::string_view(type) == "ImageLoad")
            color = ImVec4(0.55f, 0.49f, 0.96f, 1.0f);
        else
            color = ImVec4(0.25f, 0.80f, 0.49f, 1.0f);
        
        ImGui::BeginChild(type, ImVec2(150, 70), true);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("%s", type);
        ImGui::PopStyleColor();
        ImGui::Text("%zu", count);
        ImGui::EndChild();

        if (i + 1 < static_cast<int>(std::size(KNOWN_TYPES)))
            ImGui::SameLine();
    }

    ImGui::End();
}

const char* Dashboard::GetTitle() const
{
    return Name;
}

bool Dashboard::HasBadge() const
{
    auto it = _Cache.find("RemoteThread");
    return it != _Cache.end() && it->second > 0;
}

void Dashboard::RefreshCache()
{
    _Cache.clear();
    auto records = _EventRepository->QueryAll();

    for (auto& record : records)
    {
        _Cache[record.Type] += 1;
    }
}