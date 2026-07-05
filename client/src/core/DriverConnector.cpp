#include "core/DriverConnector.hpp"

/**
 * @file DriverConnector.cpp
 * @brief Implements DriverConnector using the Win32 CreateFile/ReadFile APIs.
 */

/** @brief Closes the device handle if still open. */
DriverConnector::~DriverConnector()
{
    Disconnect();
}

/** @brief Opens a handle to the driver device (e.g. L"\\\\.\\SysMon"). */
bool DriverConnector::Connect(std::wstring_view devicePath) noexcept
{
    if (IsConnected())
    {
        return true;
    }

    _File = CreateFileW(
        devicePath.data(), 
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    return _File != INVALID_HANDLE_VALUE;
}

/** @brief Closes the device handle. */
void DriverConnector::Disconnect() noexcept
{
    if (IsConnected())
    {
        CloseHandle(_File);
        _File = INVALID_HANDLE_VALUE;
    }
}

/** @brief Reads pending kernel events into @p buffer via ReadFile. */
bool DriverConnector::Read(void* buffer, DWORD size, DWORD& bytesRead) noexcept
{
    if (!IsConnected())
    {
        return false;
    }

    return ReadFile(_File, buffer, size, &bytesRead, nullptr);
}

/** @brief Returns true while a device handle is open. */
bool DriverConnector::IsConnected() const noexcept
{
    return _File != INVALID_HANDLE_VALUE;
}