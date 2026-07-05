#pragma once

#include "interfaces/IDriverConnector.hpp"

/**
 * @file DriverConnector.hpp
 * @brief Declares DriverConnector, the concrete IDriverConnector implementation
 *        used to communicate with the SysMon kernel driver.
 */

/**
 * @brief IDriverConnector implementation using CreateFile/ReadFile
 *        against the SysMon device symbolic link.
 */
class DriverConnector : public IDriverConnector
{
public:
    /** @brief Closes the device handle if still open. */
    ~DriverConnector() override;

    /**
     * @brief Opens a handle to the driver device (e.g. L"\\\\.\\SysMon").
     * @param devicePath Path of the device symbolic link to open.
     * @return true if the device handle was successfully opened.
     */
    bool Connect(std::wstring_view devicePath) noexcept override;

    /** @brief Closes the device handle. */
    void Disconnect() noexcept override;

    /**
     * @brief Reads pending kernel events into @p buffer via ReadFile.
     * @param buffer Destination buffer receiving the raw event data.
     * @param size Capacity of @p buffer, in bytes.
     * @param bytesRead Receives the number of bytes actually read.
     * @return true if the read succeeded.
     */
    bool Read(void* buffer, DWORD size, DWORD& bytesRead) noexcept override;

    /**
     * @brief Returns true while a device handle is open.
     * @return true if the device is currently connected.
     */
    bool IsConnected() const noexcept override;

private:
    /** @brief Handle to the opened driver device, or INVALID_HANDLE_VALUE when disconnected. */
    HANDLE _File = INVALID_HANDLE_VALUE;
};
