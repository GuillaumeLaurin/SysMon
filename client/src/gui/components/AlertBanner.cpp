#include "gui/components/AlertBanner.hpp"

#include <imgui.h>

/** @brief Sets the message to display; an empty message hides the banner. */
void AlertBanner::SetMessage(std::string message)
{
    _Message = std::move(message);
}

/** @brief Hides the banner by clearing its message. */
void AlertBanner::Clear()
{
    _Message.clear();
}

/** @brief Updates the component state; no-op, the banner has no per-frame state to refresh. */
void AlertBanner::Update()
{
}

/** @brief Draws the banner when a message is set. */
void AlertBanner::Render()
{
    if (_Message.empty())
        return;

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.32f, 0.29f, 1.0f));
    ImGui::Text("%s", _Message.c_str());
    ImGui::PopStyleColor();
    ImGui::Separator();
}
