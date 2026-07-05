/**
 * @file IDriverService.hpp
 * @brief Contract for controlling the SysMon driver service via the Windows SCM.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

/**
 * @brief Controls the SysMon driver service through the Windows Service Control Manager.
 */
class IDriverService
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IDriverService() = default;

    /**
     * @brief Starts the driver service (no-op if already running).
     * @return true on success, false if the service could not be started.
     */
    virtual bool Start() noexcept = 0;

    /**
     * @brief Stops the driver service.
     * @return true on success, false if the service could not be stopped.
     */
    virtual bool Stop() noexcept = 0;

    /**
     * @brief Returns true if the service is currently running.
     * @return true if the SCM reports the service as running.
     */
    virtual bool IsRunning() noexcept = 0;
};
