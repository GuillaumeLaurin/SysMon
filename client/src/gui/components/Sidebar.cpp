#include "gui/components/Sidebar.hpp"

#include <imgui.h>

constexpr float SIDEBAR_WIDTH = 180.0f;

/** @brief Constructs the sidebar with its router and page manager dependencies. */
Sidebar::Sidebar(std::shared_ptr<IRouter> router, std::shared_ptr<IPageManager> pageManager)
    : _Router(router), _PageManager(pageManager)
{
}

/** @brief Appends a navigation entry for the page with the given identifier. */
void Sidebar::AddEntry(const std::string& pageId)
{
    _Entries.push_back(pageId);
}

/** @brief Updates the component state; no-op, entries are managed via AddEntry(). */
void Sidebar::Update()
{
}

/**
 * @brief Draws the sidebar and handles navigation clicks.
 * @note Renders its own fixed, borderless window pinned to the left edge of
 *       the viewport, sized to the full display height.
 */
void Sidebar::Render()
{
    const ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(SIDEBAR_WIDTH, io.DisplaySize.y), ImGuiCond_Always);

    constexpr ImGuiWindowFlags FLAGS =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("##Sidebar", nullptr, FLAGS);

    ImGui::TextUnformatted("SysMon");
    ImGui::Separator();

    auto currentPage = _Router->CurrentPage();

    for (const auto& pageId : _Entries)
    {
        auto page = _PageManager->GetPage(pageId);

        if (!page)
            continue;

        std::string label = page->GetTitle();

        if (page->HasBadge())
            label += " (!)";

        bool selected = (page == currentPage);

        if (ImGui::Selectable(label.c_str(), selected) && !selected)
            _Router->Navigate(pageId);
    }

    ImGui::End();
}

/** @brief Sidebar width in pixels, used to lay out the page area. */
float Sidebar::Width() const
{
    return SIDEBAR_WIDTH;
}
