/**
 * @file Settings.hpp
 * @brief Settings page: refresh interval, max rows, and event-history cleanup.
 */

#pragma once

#include "interfaces/IPage.hpp"
#include "interfaces/IConfigRepository.hpp"
#include "interfaces/IEventRepository.hpp"

#include "macros/SysMonMacros.hpp"

#include <chrono>
#include <memory>
#include <string>

/**
 * @brief Settings page: refresh interval, max rows, and event-history cleanup.
 */
class Settings : public IPage
{
    _PAGE_NAME "Settings";
    SYSMON_CLASS(Settings);
public:
    /**
     * @brief Constructs the page with its config and event repository dependencies.
     * @param configRepository Persists and reloads the refresh interval and max row settings.
     * @param eventRepository Used to purge the stored event history.
     */
    Settings(
        std::shared_ptr<IConfigRepository> configRepository,
        std::shared_ptr<IEventRepository>  eventRepository);

    /**
     * @brief Default destructor.
     */
    ~Settings() override = default;

    /**
     * @brief Loads the current values from the config repository.
     */
    void OnEnter() override;

    /**
     * @brief Clears the transient status message when the page becomes inactive.
     */
    void OnExit() override;

    /**
     * @brief Expires the transient status message once its display duration elapses.
     */
    void Update() override;

    /**
     * @brief Draws the settings form and the save/clear actions.
     * @note Wraps its content in an ImGui::Begin()/End() window pair and uses
     *       an ImGui popup modal to confirm event purging.
     */
    void Render() override;

    /**
     * @brief Human-readable title shown in the sidebar/header.
     * @return The page's static name.
     */
    const char* GetTitle() const override;

    /**
     * @brief Base implementation; always false, Settings never shows a badge.
     * @return False.
     */
    bool        HasBadge() const override;

private:
    /** Persists and reloads the refresh interval and max row settings. */
    std::shared_ptr<IConfigRepository> _ConfigRepository;
    /** Used to purge the stored event history. */
    std::shared_ptr<IEventRepository>  _EventRepository;

    /** Refresh interval form field, in milliseconds. */
    int _RefreshIntervalMs;
    /** Maximum row count form field. */
    int _MaxRows;

    /** Time point at which the transient status message should be cleared. */
    std::chrono::steady_clock::time_point _StatusUntil;
    /** Transient status message shown under the form (e.g. "Settings saved."). */
    std::string                           _StatusMessage;

    /**
     * @brief Reads the settings from the config repository into the form fields.
     */
    void LoadConfig();

    /**
     * @brief Persists the form fields into the config repository.
     */
    void SaveConfig();

    /**
     * @brief Shows a transient status message under the form.
     * @param message Text to display until it expires.
     */
    void ShowStatus(std::string message);
};
