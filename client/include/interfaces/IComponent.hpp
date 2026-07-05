#pragma once

/**
 * @brief Base contract for reusable UI building blocks (cards, tables, banners...).
 */
class IComponent
{
public:
    virtual ~IComponent() = default;

    /**
     * @brief Updates the component state; called once per frame before Render().
     */
    virtual void Update() = 0;

    /**
     * @brief Draws the component with ImGui; called once per frame.
     */
    virtual void Render() = 0;
};
