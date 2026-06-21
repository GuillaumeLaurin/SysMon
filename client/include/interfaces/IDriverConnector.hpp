#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <string>

class IDriverConnector
{
public:
    virtual ~IDriverConnector() = default;

    virtual bool Connect(std::wstring_view devicePath) noexcept = 0;
    virtual void Disconnect() noexcept = 0;
    virtual bool Read(void* buffer, DWORD size, DWORD& bytesRead) noexcept = 0;
    virtual bool IsConnected() const noexcept = 0;
};