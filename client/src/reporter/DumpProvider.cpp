#include "reporter/DumpProvider.hpp"

#include <dbghelp.h>

#include <string>

#include <filesystem>

DumpProvider::DumpProvider()
    : _CurrentProcess(GetCurrentProcess())
{
}

bool DumpProvider::GenerateDump(
    const ErrorRecord& trigger,
    std::wstring_view  outputPath,
    DumpType           type
) noexcept
{
    MINIDUMP_TYPE miniType;

    switch(type)
    {
        case DumpType::Mini:
        {
            miniType = MiniDumpNormal;
            break;
        }
        case DumpType::Full:
        {
            miniType = MiniDumpWithFullMemory;
            break;
        }
        default:
        {
            miniType = MiniDumpWithPrivateReadWriteMemory;
        }
    }

    auto dumpFile = std::filesystem::path(outputPath) / (trigger.Fingerprint + ".dmp");

    std::filesystem::create_directories(
        dumpFile.parent_path()
    );
    
    HANDLE hFile = CreateFileW(
        dumpFile.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    bool status = MiniDumpWriteDump(
        _CurrentProcess,
        GetCurrentProcessId(),
        hFile,
        miniType,
        nullptr,
        nullptr,
        nullptr
    );

    CloseHandle(hFile);

    return  status;
}

bool DumpProvider::IsSupported() const noexcept
{
    return true;
}