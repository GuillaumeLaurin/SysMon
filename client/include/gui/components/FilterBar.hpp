/**
 * @file FilterBar.hpp
 * @brief Text search box plus event-type combo used to filter event tables.
 */

#pragma once

#include "interfaces/IComponent.hpp"

#include "storage/EventRecord.hpp"

#include <string>
#include <vector>

/**
 * @brief Text search box plus event-type combo used to filter event tables.
 *
 * @note Filter state (text and selected type) is kept as member fields and
 *       persists across frames; Render() draws the current state and lets
 *       the user mutate it via ImGui widgets.
 */
class FilterBar : public IComponent
{
public:
    /**
     * @brief Constructs the filter bar.
     * @param id ImGui identifier used to scope the internal widgets.
     * @param types Selectable event types; an implicit "All types" entry is added.
     */
    FilterBar(std::string id, std::vector<std::string> types);

    /**
     * @brief Default destructor.
     */
    ~FilterBar() override = default;

    /**
     * @brief Updates the component state; called once per frame before Render().
     */
    void Update() override;

    /**
     * @brief Draws the search box and the type combo.
     * @note Must be called between the page's ImGui::Begin()/End() calls.
     */
    void Render() override;

    /**
     * @brief Returns true if the record passes the current text and type filters.
     * @param record Event record to test.
     * @return True when the record matches the selected type and search text.
     */
    bool Matches(const EventRecord& record) const;

private:
    /** ImGui identifier scoping this filter bar's widgets. */
    std::string              _Id;
    /** Selectable event types (excluding the implicit "All types" entry). */
    std::vector<std::string> _Types;
    /** Backing buffer for the ImGui text input. */
    char                     _Text[256];
    /** Selected type index; 0 means "All types", N means _Types[N - 1]. */
    int                      _TypeIndex;
};
