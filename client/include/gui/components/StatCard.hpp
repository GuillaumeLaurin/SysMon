/**
 * @file StatCard.hpp
 * @brief Small colored ImGui card showing a label and a numeric statistic.
 */

#pragma once

#include "interfaces/IComponent.hpp"

#include <imgui.h>

#include <string>
#include <cstddef>

/**
 * @brief Small colored card showing a label and a numeric statistic (dashboard).
 */
class StatCard : public IComponent
{
public:
    /**
     * @brief Constructs the card.
     * @param label Caption of the card, also used as the ImGui child window id.
     * @param color Accent color applied to the label text.
     */
    StatCard(std::string label, const ImVec4& color);

    /**
     * @brief Default destructor.
     */
    ~StatCard() override = default;

    /**
     * @brief Sets the numeric value displayed by the card.
     * @param value New value to display.
     */
    void SetValue(std::size_t value);

    /**
     * @brief Updates the component state; called once per frame before Render().
     */
    void Update() override;

    /**
     * @brief Draws the card.
     * @note Must be called between the page's ImGui::Begin()/End() calls.
     */
    void Render() override;

    /**
     * @brief Caption of the card.
     * @return Reference to the card's label.
     */
    const std::string& Label() const;

private:
    /** Caption displayed on the card and used as its ImGui child window id. */
    std::string _Label;
    /** Accent color applied to the label text. */
    ImVec4      _Color;
    /** Numeric value currently displayed. */
    std::size_t _Value;
};
