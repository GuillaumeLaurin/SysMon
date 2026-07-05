#include "core/DriverService.hpp"

/**
 * @file DriverService.cpp
 * @brief Implements DriverService using the Windows Service Control Manager APIs.
 */

/** @brief Maximum time to wait for a service state transition, in milliseconds. */
constexpr DWORD STATE_TIMEOUT_MS  = 10000;
/** @brief Delay between two service status polls, in milliseconds. */
constexpr DWORD POLL_INTERVAL_MS  = 100;

/** @brief Constructs the driver service controller and derives the ASCII service name used for logging. */
DriverService::DriverService(std::wstring serviceName, std::shared_ptr<ILogger> logger)
    : _ServiceName(std::move(serviceName)), _Logger(logger)
{
    _ServiceNameA.reserve(_ServiceName.size());

    for (wchar_t c : _ServiceName)
        _ServiceNameA.push_back(c < 128 ? static_cast<char>(c) : '?');
}

/** @brief Starts the driver service (no-op if already running). */
bool DriverService::Start() noexcept
{
    SC_HANDLE service = OpenDriverService(SERVICE_START | SERVICE_QUERY_STATUS);

    if (!service)
        return false;

    bool ok = true;

    if (!StartServiceW(service, 0, nullptr))
    {
        auto error = GetLastError();

        if (error != ERROR_SERVICE_ALREADY_RUNNING)
        {
            LOG_ERROR(*_Logger, "StartService failed for '{}': {}", _ServiceNameA, error);
            ok = false;
        }
    }

    if (ok)
    {
        ok = WaitForState(service, SERVICE_RUNNING);

        if (ok)
            LOG_INFO(*_Logger, "Driver service started.");
    }

    CloseServiceHandle(service);
    return ok;
}

/** @brief Stops the driver service and waits for it to reach the stopped state. */
bool DriverService::Stop() noexcept
{
    SC_HANDLE service = OpenDriverService(SERVICE_STOP | SERVICE_QUERY_STATUS);

    if (!service)
        return false;

    bool ok = true;
    SERVICE_STATUS status{};

    if (!ControlService(service, SERVICE_CONTROL_STOP, &status))
    {
        auto error = GetLastError();

        if (error != ERROR_SERVICE_NOT_ACTIVE)
        {
            LOG_ERROR(*_Logger, "ControlService(stop) failed for '{}': {}", _ServiceNameA, error);
            ok = false;
        }
    }

    if (ok)
    {
        ok = WaitForState(service, SERVICE_STOPPED);

        if (ok)
            LOG_INFO(*_Logger, "Driver service stopped.");
    }

    CloseServiceHandle(service);
    return ok;
}

/** @brief Returns true if the service is currently running. */
bool DriverService::IsRunning() noexcept
{
    SC_HANDLE service = OpenDriverService(SERVICE_QUERY_STATUS);

    if (!service)
        return false;

    SERVICE_STATUS_PROCESS status{};
    DWORD needed = 0;

    bool running =
        QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
            reinterpret_cast<LPBYTE>(&status), sizeof(status), &needed) &&
        status.dwCurrentState == SERVICE_RUNNING;

    CloseServiceHandle(service);
    return running;
}

/** @brief Opens the SCM and the service with the requested access rights. */
SC_HANDLE DriverService::OpenDriverService(DWORD access) noexcept
{
    SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);

    if (!scm)
    {
        LOG_ERROR(*_Logger, "OpenSCManager failed: {} (droits administrateur requis)", GetLastError());
        return nullptr;
    }

    SC_HANDLE service = OpenServiceW(scm, _ServiceName.c_str(), access);

    if (!service)
    {
        LOG_ERROR(*_Logger, "OpenService failed for '{}': {}", _ServiceNameA, GetLastError());
    }

    CloseServiceHandle(scm);
    return service;
}

/** @brief Polls the service status until @p desiredState is reached or a timeout expires. */
bool DriverService::WaitForState(SC_HANDLE service, DWORD desiredState) noexcept
{
    SERVICE_STATUS_PROCESS status{};
    DWORD needed  = 0;
    DWORD elapsed = 0;

    while (elapsed < STATE_TIMEOUT_MS)
    {
        if (!QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
                reinterpret_cast<LPBYTE>(&status), sizeof(status), &needed))
        {
            LOG_ERROR(*_Logger, "QueryServiceStatusEx failed: {}", GetLastError());
            return false;
        }

        if (status.dwCurrentState == desiredState)
            return true;

        Sleep(POLL_INTERVAL_MS);
        elapsed += POLL_INTERVAL_MS;
    }

    LOG_ERROR(*_Logger, "Timeout waiting for service state {}", desiredState);
    return false;
}
