#include <Windows.h>

#include <stdio.h>

#include <memory>

#include <string>

#include "Public.h"

#define SLEEP_CONSTANT 400

INT Error(_In_ CONST CHAR* msg);

VOID DisplayInfo(BYTE* buffer, DWORD size);

VOID DisplayTime(CONST LARGE_INTEGER& time);

int main()
{
    auto hFile = CreateFileW(L"\\\\.\\SysMon", GENERIC_READ, 0, 
        nullptr, OPEN_EXISTING, 0, nullptr);
    
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return Error("Failed to open file");
    }

    int size = 1 << 16;
    auto buffer = std::make_unique<BYTE[]>(size);

    while (true)
    {
        DWORD bytes = 0;
        // error handling omitted
        ReadFile(hFile, buffer.get(), size, &bytes, nullptr); 

        if (bytes)
        {
            DisplayInfo(buffer.get(), bytes);
        }

        Sleep(SLEEP_CONSTANT);
    }

    CloseHandle(hFile);
    return 0;
}

INT Error(_In_ CONST CHAR* msg)
{
    printf("%s error=%u\n", msg, ::GetLastError());
    
    return 1;
}

VOID DisplayInfo(BYTE* buffer, DWORD size)
{
    while (size > 0)
    {
        auto header = (ItemHeader*)buffer;

        switch(header->Type)
        {
            case ItemType::ProcessExit:
            {
                DisplayTime(header->Time);
                auto info = (ProcessExitInfo*)buffer;
                printf("Process %u Exited (Code: %u)\n",
                    info->ProcessId, info->ExitCode);
                break;
            }
            case ItemType::ProcessCreate:
            {
                DisplayTime(header->Time);
                auto info = (ProcessCreateInfo*)buffer;
                std::wstring commandLine(info->CommandLine, 
                    info->CommandLineLength);
                printf("Process %u Created. Command line: %ws\n",
                    info->ProcessId, commandLine.c_str());
                break;
            }
            case ItemType::ThreadCreate:
            {
                DisplayTime(header->Time);
                auto info = (ThreadCreateInfo*)buffer;
                printf("Thread %u Created in process %u\n",
                    info->ThreadId, info->ProcessId);
                break;
            }
            case ItemType::ThreadExit:
            {
                DisplayTime(header->Time);
                auto info = (ThreadExitInfo*)buffer;
                printf("Thread %u Exited from process %u (Code: %u)",
                    info->ThreadId, info->ProcessId, info->ExitCode);
                break;
            }
        }
        buffer += header->Size;
        size -= header->Size;
    }
}

VOID DisplayTime(CONST LARGE_INTEGER& time)
{
    FILETIME local;

    FileTimeToLocalFileTime((FILETIME*)&time, &local);
    SYSTEMTIME st;
    FileTimeToSystemTime(&local, &st);
    printf("%02d:%02d:%02d.%03d: ",
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}