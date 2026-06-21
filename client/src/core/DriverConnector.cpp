#include "core/DriverConnector.hpp"

DriverConnector::~DriverConnector()
{
    Disconnect();
}

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

void DriverConnector::Disconnect() noexcept
{
    if (IsConnected())
    {
        CloseHandle(_File);
        _File = INVALID_HANDLE_VALUE;
    }
}

bool DriverConnector::Read(void* buffer, DWORD size, DWORD& bytesRead) noexcept
{
    if (!IsConnected())
    {
        return false;
    }

    return ReadFile(_File, buffer, size, &bytesRead, nullptr);
}

bool DriverConnector::IsConnected() const noexcept
{
    return _File != INVALID_HANDLE_VALUE;
}