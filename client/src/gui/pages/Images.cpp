#include "gui/pages/Images.hpp"

/** @brief Constructs the page, filtering the event table to ImageLoad events. */
Images::Images(
    std::shared_ptr<IEventRepository>  eventRepository,
    std::shared_ptr<IConfigRepository> configRepository
)   : EventTablePage(
        eventRepository,
        configRepository,
        { "ImageLoad" },
        "Image path")
{
}

/** @brief Human-readable title shown in the sidebar/header. */
const char* Images::GetTitle() const
{
    return Name;
}
