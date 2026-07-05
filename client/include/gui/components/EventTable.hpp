/**
 * @file EventTable.hpp
 * @brief Sortable ImGui table listing event records (time, type, PID, data).
 */

#pragma once

#include "interfaces/IComponent.hpp"

#include "storage/EventRecord.hpp"

#include <functional>
#include <string>
#include <vector>

/**
 * @brief Sortable ImGui table listing event records (time, type, PID, data).
 *
 * @note Rows are drawn with an ImGuiListClipper, so Render() stays cheap
 *       even with a large number of events.
 */
class EventTable : public IComponent
{
public:
    /**
     * @brief Constructs the table.
     * @param id ImGui identifier used for the underlying table widget.
     * @param dataHeader Label of the last ("data") column.
     */
    EventTable(std::string id, std::string dataHeader = "Data");

    /**
     * @brief Default destructor.
     */
    ~EventTable() override = default;

    /**
     * @brief Replaces the rows displayed by the table.
     * @param events New set of event records to display.
     */
    void SetEvents(std::vector<EventRecord> events);

    /**
     * @brief Sets a predicate; matching rows are drawn highlighted.
     * @param predicate Returns true when a given record should be highlighted.
     */
    void SetHighlight(std::function<bool(const EventRecord&)> predicate);

    /**
     * @brief Updates the component state; called once per frame before Render().
     */
    void Update() override;

    /**
     * @brief Draws the table.
     * @note Must be called between the page's ImGui::Begin()/End() calls.
     */
    void Render() override;

private:
    /** ImGui identifier of the table widget. */
    std::string                             _Id;
    /** Label of the last (data) column. */
    std::string                             _DataHeader;
    /** Rows currently displayed. */
    std::vector<EventRecord>                _Events;
    /** Optional predicate used to highlight matching rows. */
    std::function<bool(const EventRecord&)> _Highlight;

    /**
     * @brief Converts a kernel LARGE_INTEGER timestamp to a readable local time string.
     * @param timestamp Kernel timestamp to convert.
     * @return Local time formatted as "HH:MM:SS.mmm", or "-" on conversion failure.
     */
    static std::string FormatTimestamp(const LARGE_INTEGER& timestamp);
};
