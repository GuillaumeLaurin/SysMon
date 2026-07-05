/**
 * @file AlertBanner.hpp
 * @brief Dismissible ImGui banner shown at the top of a page to surface alerts.
 */

#pragma once

#include "interfaces/IComponent.hpp"

#include <string>

/**
 * @brief Dismissible banner shown at the top of a page to surface alerts.
 *
 * @note The banner draws nothing when its message is empty; call SetMessage()
 *       to show it and Clear() to hide it again.
 */
class AlertBanner : public IComponent
{
public:
    /**
     * @brief Constructs an empty (hidden) banner.
     */
    AlertBanner() = default;

    /**
     * @brief Default destructor.
     */
    ~AlertBanner() override = default;

    /**
     * @brief Sets the message to display; an empty message hides the banner.
     * @param message Text shown in the banner.
     */
    void SetMessage(std::string message);

    /**
     * @brief Hides the banner by clearing its message.
     */
    void Clear();

    /**
     * @brief Updates the component state; called once per frame before Render().
     */
    void Update() override;

    /**
     * @brief Draws the banner when a message is set.
     * @note Must be called between the page's ImGui::Begin()/End() calls.
     */
    void Render() override;

private:
    /** Current banner text; empty means the banner is hidden. */
    std::string _Message;
};
