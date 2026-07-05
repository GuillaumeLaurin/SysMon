#include "gui/pages/Processes.hpp"

/** @brief Constructs the page, filtering the event table to process creation/exit events. */
Processes::Processes(
    std::shared_ptr<IEventRepository>  eventRepository,
    std::shared_ptr<IConfigRepository> configRepository
)   : EventTablePage(
        eventRepository,
        configRepository,
        { "ProcessCreate", "ProcessExit" },
        "Command line / Exit code")
{
}

/** @brief Human-readable title shown in the sidebar/header. */
const char* Processes::GetTitle() const
{
    return Name;
}
