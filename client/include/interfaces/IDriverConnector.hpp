/**
 * @file IDriverConnector.hpp
 * @brief Contract for the user-mode connection to the SysMon kernel driver device.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <string>

/**
 * @brief Handles the user-mode connection to the SysMon kernel driver device.
 */
class IDriverConnector
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     * @note Implementations should disconnect if still connected.
     */
    virtual ~IDriverConnector() = default;

    /**
     * @brief Opens a handle to the driver device (e.g. L"\\\\.\\SysMon").
     * @param devicePath Win32 device path of the driver.
     * @return true on success, false if the device could not be opened.
     */
    virtual bool Connect(std::wstring_view devicePath) noexcept = 0;

    /**
     * @brief Closes the device handle.
     */
    virtual void Disconnect() noexcept = 0;

    /**
     * @brief Reads pending kernel events into the provided buffer.
     * @param buffer Destination buffer to receive raw event data.
     * @param size Capacity of @p buffer, in bytes.
     * @param bytesRead Receives the number of bytes actually read.
     * @return true on success, false on I/O error or when not connected.
     */
    virtual bool Read(void* buffer, DWORD size, DWORD& bytesRead) noexcept = 0;

    /**
     * @brief Returns true while a device handle is open.
     * @return true if the connector currently holds an open device handle.
     */
    virtual bool IsConnected() const noexcept = 0;
};
