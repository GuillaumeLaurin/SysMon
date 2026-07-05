#include "gui/pages/Settings.hpp"

#include "gui/ConfigKeys.hpp"

#include <imgui.h>

#include <algorithm>
#include <cstdlib>
#include <string>

constexpr auto STATUS_DURATION = std::chrono::seconds(3);

/** @brief Constructs the page with its config and event repository dependencies. */
Settings::Settings(
    std::shared_ptr<IConfigRepository> configRepository,
    std::shared_ptr<IEventRepository>  eventRepository
)   : _ConfigRepository(configRepository)
    , _EventRepository(eventRepository)
    , _RefreshIntervalMs(500)
    , _MaxRows(1000)
{
}

/** @brief Loads the current values from the config repository. */
void Settings::OnEnter()
{
    LoadConfig();
}

/** @brief Clears the transient status message when the page becomes inactive. */
void Settings::OnExit()
{
    _StatusMessage.clear();
}

/** @brief Expires the transient status message once its display duration elapses. */
void Settings::Update()
{
    if (!_StatusMessage.empty() && std::chrono::steady_clock::now() > _StatusUntil)
        _StatusMessage.clear();
}

/** @brief Draws the settings form and the save/clear actions. */
void Settings::Render()
{
    ImGui::Begin(GetTitle());

    ImGui::TextUnformatted("Display");
    ImGui::Separator();

    ImGui::SetNextItemWidth(200);
    ImGui::InputInt("Refresh interval (ms)", &_RefreshIntervalMs, 50, 250);

    ImGui::SetNextItemWidth(200);
    ImGui::InputInt("Maximum number of rows", &_MaxRows, 100, 500);

    _RefreshIntervalMs = std::clamp(_RefreshIntervalMs, 100, 10000);
    _MaxRows           = std::clamp(_MaxRows, 100, 100000);

    if (ImGui::Button("Save"))
        SaveConfig();

    ImGui::SameLine();

    if (ImGui::Button("Reset"))
    {
        LoadConfig();
        ShowStatus("Values reloaded.");
    }

    ImGui::Spacing();
    ImGui::TextUnformatted("Data");
    ImGui::Separator();

    if (ImGui::Button("Purge events..."))
        ImGui::OpenPopup("ConfirmClear");

    if (ImGui::BeginPopupModal("ConfirmClear", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Permanently delete all captured events?");
        ImGui::Separator();

        if (ImGui::Button("Delete", ImVec2(120, 0)))
        {
            ShowStatus(_EventRepository->Clear()
                ? "Events purged."
                : "Failed to purge events.");
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    if (!_StatusMessage.empty())
    {
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.25f, 0.80f, 0.49f, 1.0f));
        ImGui::TextUnformatted(_StatusMessage.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::End();
}

/** @brief Human-readable title shown in the sidebar/header. */
const char* Settings::GetTitle() const
{
    return Name;
}

/** @brief Base implementation; always false, Settings never shows a badge. */
bool Settings::HasBadge() const
{
    return false;
}

/** @brief Reads the settings from the config repository into the form fields. */
void Settings::LoadConfig()
{
    auto refresh = _ConfigRepository->Get(ConfigKeys::RefreshIntervalMs, ConfigKeys::DefaultRefreshIntervalMs);
    auto maxRows = _ConfigRepository->Get(ConfigKeys::MaxRows, ConfigKeys::DefaultMaxRows);

    _RefreshIntervalMs = std::atoi(refresh.c_str());
    _MaxRows           = std::atoi(maxRows.c_str());

    if (_RefreshIntervalMs <= 0)
        _RefreshIntervalMs = std::atoi(ConfigKeys::DefaultRefreshIntervalMs);

    if (_MaxRows <= 0)
        _MaxRows = std::atoi(ConfigKeys::DefaultMaxRows);
}

/** @brief Persists the form fields into the config repository. */
void Settings::SaveConfig()
{
    bool ok = _ConfigRepository->Set(ConfigKeys::RefreshIntervalMs, std::to_string(_RefreshIntervalMs)) &&
              _ConfigRepository->Set(ConfigKeys::MaxRows, std::to_string(_MaxRows));

    ShowStatus(ok ? "Settings saved." : "Failed to save settings.");
}

/** @brief Shows a transient status message under the form. */
void Settings::ShowStatus(std::string message)
{
    _StatusMessage = std::move(message);
    _StatusUntil   = std::chrono::steady_clock::now() + STATUS_DURATION;
}
