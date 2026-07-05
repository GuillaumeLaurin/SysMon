#include "gui/components/FilterBar.hpp"

#include <imgui.h>

#include <algorithm>
#include <cctype>

namespace
{
    std::string ToLower(std::string_view input)
    {
        std::string result(input);
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return result;
    }
}

/** @brief Constructs the filter bar with its ImGui identifier and selectable types. */
FilterBar::FilterBar(std::string id, std::vector<std::string> types)
    : _Id(std::move(id)), _Types(std::move(types)), _Text{}, _TypeIndex(0)
{
}

/** @brief Updates the component state; no-op, filter state is mutated directly by Render(). */
void FilterBar::Update()
{
}

/** @brief Draws the search box and the type combo. */
void FilterBar::Render()
{
    ImGui::PushID(_Id.c_str());

    ImGui::SetNextItemWidth(250);
    ImGui::InputTextWithHint("##Filter", "Filter (pid, tid or data)...", _Text, sizeof(_Text));

    if (_Types.size() > 1)
    {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(180);

        const char* preview = _TypeIndex == 0 ? "All types" : _Types[_TypeIndex - 1].c_str();

        if (ImGui::BeginCombo("##Type", preview))
        {
            if (ImGui::Selectable("All types", _TypeIndex == 0))
                _TypeIndex = 0;

            for (int i = 0; i < static_cast<int>(_Types.size()); ++i)
            {
                if (ImGui::Selectable(_Types[i].c_str(), _TypeIndex == i + 1))
                    _TypeIndex = i + 1;
            }

            ImGui::EndCombo();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        _Text[0]   = '\0';
        _TypeIndex = 0;
    }

    ImGui::PopID();
    ImGui::Separator();
}

/** @brief Returns true if the record passes the current text and type filters. */
bool FilterBar::Matches(const EventRecord& record) const
{
    if (_TypeIndex > 0 && record.Type != _Types[_TypeIndex - 1])
        return false;

    if (_Text[0] == '\0')
        return true;

    auto needle = ToLower(_Text);

    if (ToLower(record.Data).find(needle) != std::string::npos)
        return true;

    if (std::to_string(record.Pid).find(needle) != std::string::npos)
        return true;

    return std::to_string(record.Tid).find(needle) != std::string::npos;
}
