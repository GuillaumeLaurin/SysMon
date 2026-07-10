#include "gui/pages/Registry.hpp"

/** @brief Constructs the page, filtering the event table to RegistrySetValue events. */
Registry::Registry(
    std::shared_ptr<IEventRepository>  eventRepository,
    std::shared_ptr<IConfigRepository> configRepository
)   : EventTablePage(
        eventRepository,
        configRepository,
        { "RegistrySetValue" },
        "Registry data")
{
}

/** @brief Human-readable title shown in the sidebar/header. */
const char* Registry::GetTitle() const
{
    return Name;
}
