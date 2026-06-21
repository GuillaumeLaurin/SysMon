#pragma once

#include "interfaces/IDriverConnector.hpp"

class DriverConnector : public IDriverConnector
{
public:
    ~DriverConnector() override;

    bool Connect(std::wstring_view devicePath) noexcept override;
    void Disconnect() noexcept override;
    bool Read(void* buffer, DWORD size, DWORD& bytesRead) noexcept override;
    bool IsConnected() const noexcept override;

private:
    HANDLE _File = INVALID_HANDLE_VALUE;
};