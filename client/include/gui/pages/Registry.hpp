/**
 * @file Registry.hpp
 * @brief Page listing registry set-value events captured by the driver.
 */

#pragma once

#include "gui/pages/EventTablePage.hpp"

#include "macros/SysMonMacros.hpp"

/**
 * @brief Page listing registry set-value events (key, value name, type and data).
 */
class Registry : public EventTablePage
{
    _PAGE_NAME "Registry";
    SYSMON_CLASS(Registry);
public:
    /**
     * @brief Constructs the page, filtering the event table to RegistrySetValue events.
     * @param eventRepository Source of the displayed events.
     * @param configRepository Provides refresh interval and max row count.
     */
    Registry(
        std::shared_ptr<IEventRepository>  eventRepository,
        std::shared_ptr<IConfigRepository> configRepository);

    /**
     * @brief Default destructor.
     */
    ~Registry() override = default;

    /**
     * @brief Human-readable title shown in the sidebar/header.
     * @return The page's static name.
     */
    const char* GetTitle() const override;
};
