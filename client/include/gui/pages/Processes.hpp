/**
 * @file Processes.hpp
 * @brief Page listing process creation and exit events.
 */

#pragma once

#include "gui/pages/EventTablePage.hpp"

#include "macros/SysMonMacros.hpp"

/**
 * @brief Page listing process creation and exit events.
 */
class Processes : public EventTablePage
{
    _PAGE_NAME "Processes";
    SYSMON_CLASS(Processes);
public:
    /**
     * @brief Constructs the page, filtering the event table to process creation/exit events.
     * @param eventRepository Source of the displayed events.
     * @param configRepository Provides refresh interval and max row count.
     */
    Processes(
        std::shared_ptr<IEventRepository>  eventRepository,
        std::shared_ptr<IConfigRepository> configRepository);

    /**
     * @brief Default destructor.
     */
    ~Processes() override = default;

    /**
     * @brief Human-readable title shown in the sidebar/header.
     * @return The page's static name.
     */
    const char* GetTitle() const override;
};
