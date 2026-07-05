/**
 * @file Threads.hpp
 * @brief Page listing thread creation/exit events, including remote threads.
 */

#pragma once

#include "gui/pages/EventTablePage.hpp"

#include "macros/SysMonMacros.hpp"

/**
 * @brief Page listing thread creation/exit events, including remote threads.
 */
class Threads : public EventTablePage
{
    _PAGE_NAME "Threads";
    SYSMON_CLASS(Threads);
public:
    /**
     * @brief Constructs the page, filtering the event table to thread and remote-thread events.
     * @param eventRepository Source of the displayed events.
     * @param configRepository Provides refresh interval and max row count.
     */
    Threads(
        std::shared_ptr<IEventRepository>  eventRepository,
        std::shared_ptr<IConfigRepository> configRepository);

    /**
     * @brief Default destructor.
     */
    ~Threads() override = default;

    /**
     * @brief Human-readable title shown in the sidebar/header.
     * @return The page's static name.
     */
    const char* GetTitle() const override;

    /**
     * @brief Shows a badge when remote-thread events are present.
     * @return True if at least one RemoteThread event is currently loaded.
     */
    bool HasBadge() const override;
};
