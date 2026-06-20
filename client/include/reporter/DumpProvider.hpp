#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "interfaces/IDumpProvider.hpp"

class DumpProvider : public IDumpProvider
{
public:
    DumpProvider();

    ~DumpProvider() override = default;

    bool GenerateDump(
        const ErrorRecord& trigger,
        std::wstring_view  outputPath,
        DumpType           type
    ) noexcept override;

    bool IsSupported() const noexcept override;

private:
    HANDLE _CurrentProcess;
};