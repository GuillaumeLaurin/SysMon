#include "gui/components/EventTable.hpp"

#include <imgui.h>

#include <cstdio>

/** @brief Constructs the table with its ImGui identifier and data column label. */
EventTable::EventTable(std::string id, std::string dataHeader)
    : _Id(std::move(id)), _DataHeader(std::move(dataHeader))
{
}

/** @brief Replaces the rows displayed by the table. */
void EventTable::SetEvents(std::vector<EventRecord> events)
{
    _Events = std::move(events);
}

/** @brief Sets a predicate; matching rows are drawn highlighted. */
void EventTable::SetHighlight(std::function<bool(const EventRecord&)> predicate)
{
    _Highlight = std::move(predicate);
}

/** @brief Updates the component state; no-op, rows are set externally via SetEvents(). */
void EventTable::Update()
{
}

/**
 * @brief Draws the table.
 * @note Uses an ImGuiListClipper so only visible rows are formatted/drawn per frame.
 */
void EventTable::Render()
{
    constexpr ImGuiTableFlags FLAGS =
        ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
        ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

    if (!ImGui::BeginTable(_Id.c_str(), 5, FLAGS))
        return;

    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 100.0f);
    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 110.0f);
    ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 70.0f);
    ImGui::TableSetupColumn("TID", ImGuiTableColumnFlags_WidthFixed, 70.0f);
    ImGui::TableSetupColumn(_DataHeader.c_str(), ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(_Events.size()));

    while (clipper.Step())
    {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
        {
            const auto& record = _Events[static_cast<std::size_t>(row)];
            bool highlighted   = _Highlight && _Highlight(record);

            ImGui::TableNextRow();

            if (highlighted)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.32f, 0.29f, 1.0f));

            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(FormatTimestamp(record.Timestamp).c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(record.Type.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%lu", record.Pid);
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%lu", record.Tid);
            ImGui::TableSetColumnIndex(4);
            ImGui::TextUnformatted(record.Data.c_str());

            if (highlighted)
                ImGui::PopStyleColor();
        }
    }

    ImGui::EndTable();
}

/** @brief Converts a kernel LARGE_INTEGER timestamp to a readable local time string. */
std::string EventTable::FormatTimestamp(const LARGE_INTEGER& timestamp)
{
    FILETIME fileTime;
    fileTime.dwLowDateTime  = timestamp.LowPart;
    fileTime.dwHighDateTime = static_cast<DWORD>(timestamp.HighPart);

    FILETIME localTime;
    SYSTEMTIME systemTime;

    if (!FileTimeToLocalFileTime(&fileTime, &localTime) ||
        !FileTimeToSystemTime(&localTime, &systemTime))
    {
        return "-";
    }

    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u.%03u",
        systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

    return buffer;
}
