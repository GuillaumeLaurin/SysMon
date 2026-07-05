#include "gui/pages/Threads.hpp"

#include <algorithm>

/** @brief Constructs the page, filtering the event table to thread and remote-thread events, and highlights RemoteThread rows. */
Threads::Threads(
    std::shared_ptr<IEventRepository>  eventRepository,
    std::shared_ptr<IConfigRepository> configRepository
)   : EventTablePage(
        eventRepository,
        configRepository,
        { "ThreadCreate", "ThreadExit", "RemoteThread" },
        "Exit code / Creator (pid:tid)")
{
    _Table.SetHighlight([](const EventRecord& record) {
        return record.Type == "RemoteThread";
    });
}

/** @brief Human-readable title shown in the sidebar/header. */
const char* Threads::GetTitle() const
{
    return Name;
}

/** @brief Shows a badge when remote-thread events are present. */
bool Threads::HasBadge() const
{
    return std::any_of(_Records.begin(), _Records.end(),
        [](const EventRecord& record) {
            return record.Type == "RemoteThread";
        });
}
