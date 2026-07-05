#pragma once

#include "interfaces/IDriverService.hpp"
#include "interfaces/ILogger.hpp"

#include <memory>
#include <string>

/**
 * @file DriverService.hpp
 * @brief Declares DriverService, the concrete IDriverService implementation
 *        controlling the SysMon driver through the Windows SCM.
 */

/**
 * @brief IDriverService implementation controlling the SysMon driver
 *        through the Windows Service Control Manager.
 */
class DriverService : public IDriverService
{
public:
    /**
     * @brief Constructs the driver service controller.
     * @param serviceName Name of the driver service.
     * @param logger Diagnostics logger.
     */
    DriverService(std::wstring serviceName, std::shared_ptr<ILogger> logger);

    /** @brief Default destructor. */
    ~DriverService() override = default;

    /**
     * @brief Starts the driver service (no-op if already running).
     * @return true if the service is running after the call.
     */
    bool Start() noexcept override;

    /**
     * @brief Stops the driver service and waits for it to reach the stopped state.
     * @return true if the service is stopped after the call.
     */
    bool Stop() noexcept override;

    /**
     * @brief Returns true if the service is currently running.
     * @return true if the service state is SERVICE_RUNNING.
     */
    bool IsRunning() noexcept override;

private:
    /** @brief Name of the driver service (wide string, as required by the SCM APIs). */
    std::wstring             _ServiceName;
    /** @brief ASCII copy of _ServiceName, used for logging. */
    std::string              _ServiceNameA;
    /** @brief Logger used to report SCM diagnostics. */
    std::shared_ptr<ILogger> _Logger;

    /**
     * @brief Opens the SCM and the service with the requested access rights.
     * @param access Desired access rights (SERVICE_* / SC_MANAGER_* flags).
     * @return Handle to the opened service, or nullptr on failure.
     */
    SC_HANDLE OpenDriverService(DWORD access) noexcept;

    /**
     * @brief Polls the service status until @p desiredState is reached or a timeout expires.
     * @param service Handle to the service being polled.
     * @param desiredState Target SERVICE_* state to wait for.
     * @return true if @p desiredState was reached before the timeout.
     */
    bool WaitForState(SC_HANDLE service, DWORD desiredState) noexcept;
};
