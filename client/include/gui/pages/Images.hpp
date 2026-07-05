/**
 * @file Images.hpp
 * @brief Page listing image-load events (EXE/DLL mappings).
 */

#pragma once

#include "gui/pages/EventTablePage.hpp"

#include "macros/SysMonMacros.hpp"

/**
 * @brief Page listing image-load events (EXE/DLL mappings).
 */
class Images : public EventTablePage
{
    _PAGE_NAME "Images";
    SYSMON_CLASS(Images);
public:
    /**
     * @brief Constructs the page, filtering the event table to ImageLoad events.
     * @param eventRepository Source of the displayed events.
     * @param configRepository Provides refresh interval and max row count.
     */
    Images(
        std::shared_ptr<IEventRepository>  eventRepository,
        std::shared_ptr<IConfigRepository> configRepository);

    /**
     * @brief Default destructor.
     */
    ~Images() override = default;

    /**
     * @brief Human-readable title shown in the sidebar/header.
     * @return The page's static name.
     */
    const char* GetTitle() const override;
};
