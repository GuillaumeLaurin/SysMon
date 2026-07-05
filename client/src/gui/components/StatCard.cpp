#include "gui/components/StatCard.hpp"

/** @brief Constructs the card with its label and accent color. */
StatCard::StatCard(std::string label, const ImVec4& color)
    : _Label(std::move(label)), _Color(color), _Value(0)
{
}

/** @brief Sets the numeric value displayed by the card. */
void StatCard::SetValue(std::size_t value)
{
    _Value = value;
}

/** @brief Updates the component state; no-op, the value is set externally via SetValue(). */
void StatCard::Update()
{
}

/** @brief Draws the card. */
void StatCard::Render()
{
    ImGui::BeginChild(_Label.c_str(), ImVec2(150, 70), true);
    ImGui::PushStyleColor(ImGuiCol_Text, _Color);
    ImGui::Text("%s", _Label.c_str());
    ImGui::PopStyleColor();
    ImGui::Text("%zu", _Value);
    ImGui::EndChild();
}

/** @brief Caption of the card. */
const std::string& StatCard::Label() const
{
    return _Label;
}
