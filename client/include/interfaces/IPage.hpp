#pragma once

/**
 * @brief Declares the static page identifier used for registration and routing.
 */
#ifndef _PAGE_NAME
#define _PAGE_NAME inline static const char* Name =
#endif // !_PAGE_NAME

/**
 * @brief Contract for a full-screen page of the client UI (Dashboard, Processes...).
 */
class IPage
{
public:
    virtual ~IPage() = default;

    /**
     * @brief Called when the page becomes the active page.
     */
    virtual void OnEnter() = 0;

    /**
     * @brief Called when the page stops being the active page.
     */
    virtual void OnExit() = 0;

    /**
     * @brief Updates the page state; called once per frame before Render().
     */
    virtual void Update() = 0;

    /**
     * @brief Draws the page with ImGui; called once per frame.
     */
    virtual void Render() = 0;

    /**
     * @brief Human-readable title shown in the sidebar/header.
     * @return The page title.
     */
    virtual const char* GetTitle() const = 0;

    /**
     * @brief Returns true when the sidebar entry should display a badge.
     * @return True if a badge should be shown.
     */
    virtual bool        HasBadge() const = 0;
};
