#include "core/EventProcessor.hpp"

#include "Public.h"

#include <objbase.h>

#include <intrin.h>

#include <sstream>
#include <iomanip>

#include <unordered_map>

/**
 * @file EventProcessor.cpp
 * @brief Implements EventProcessor: driver polling loop and raw event decoding.
 */

/** @brief Constructs the event processor with its dependencies. */
EventProcessor::EventProcessor(
    std::shared_ptr<IDriverConnector> connector,
    std::shared_ptr<IEventRepository> repository,
    std::shared_ptr<IErrorDispatcher> dispatcher
)   : _Connector(connector), _Repository(repository), _Dispatcher(dispatcher)
{
}

/** @brief Stops the worker thread if still running. */
EventProcessor::~EventProcessor()
{
    Stop();
}

/** @brief Starts the polling thread. */
void EventProcessor::Start() noexcept
{
    if (!_Running)
    {
        _Running = true;
        _Worker = std::thread(&EventProcessor::WorkerLoop, this);
    }
}

/** @brief Signals the polling thread to stop and joins it. */
void EventProcessor::Stop() noexcept
{
    if (_Running)
    {
        _Running = false;

        if (_Worker.joinable())
        {
            _Worker.join();
        }
    }
}

/** @brief Polling loop: reads from the driver and hands buffers to ProcessBuffer(). */
void EventProcessor::WorkerLoop()
{
    int size = 1 << 16;
    auto buffer = std::make_unique<BYTE[]>(size);

    while (_Running)
    {
        DWORD bytes = 0;

        if (_Connector->Read(buffer.get(), size, bytes) && bytes)
        {
            ProcessBuffer(buffer.get(), bytes);
        }

        Sleep(10);
    }
}

/** @brief Walks a raw driver buffer and converts each ItemHeader-based event to an EventRecord. */
void EventProcessor::ProcessBuffer(BYTE* buffer, DWORD size)
{
    while (size > 0)
    {
        EventRecord record;
        auto header = (ItemHeader*)buffer;

        record.Timestamp = header->Time;

        {
            GUID guid;
            CoCreateGuid(&guid);
            wchar_t guidStr[39];
            StringFromGUID2(guid, guidStr, 39);

            std::string id(38, '\0');

            for (int i = 0; i < 38; i++)
            {
                id[i] = static_cast<char>(guidStr[i]);  
            }

            record.Id = std::move(id);
        }

        switch(header->Type)
        {
            case ItemType::ProcessCreate:
            {
                auto info = (ProcessCreateInfo*)buffer;
                record.Type = "ProcessCreate";
                record.Pid  = info->ProcessId;
                record.Tid  = 0;
                record.Data = WStringToString(info->CommandLine);
                break;
            }
            case ItemType::ProcessExit:
            {
                auto info = (ProcessExitInfo*)buffer;
                record.Type = "ProcessExit";
                record.Pid  = info->ProcessId;
                record.Tid  = 0;
                record.Data = std::to_string(info->ExitCode);
                break;
            }
            case ItemType::ThreadCreate:
            {
                auto info = (ThreadCreateInfo*)buffer;
                record.Type = "ThreadCreate";
                record.Pid  = info->ProcessId;
                record.Tid  = info->ThreadId;
                break;
            }
            case ItemType::ThreadExit:
            {
                auto info = (ThreadExitInfo*)buffer;
                record.Type = "ThreadExit";
                record.Pid  = info->ProcessId;
                record.Tid  = info->ThreadId;
                record.Data = std::to_string(info->ExitCode);
                break;
            }
            case ItemType::ImageLoad:
            {
                auto info = (ImageLoadInfo*)buffer;
                record.Type = "ImageLoad";
                record.Pid  = info->ProcessId;
                record.Data = WStringToString(GetDosNameFromNTName(info->ImageFileName));
                break;
            }
            case ItemType::RemoteThread:
            {
                auto info = (RemoteThread*)buffer;
                record.Type = "RemoteThread";
                record.Pid  = info->ProcessId;
                record.Tid  = info->ThreadId;
                record.Data = std::to_string(info->CreatorProcessId) + ":" +
                              std::to_string(info->CreatorThreadId);
                break;
            }
            case ItemType::RegistrySetValue:
            {
                auto info = (RegistrySetValueInfo*)buffer;
                record.Type = "RegistrySetValue";
                record.Pid = info->ProcessId;
                record.Tid = info->ThreadId;

                auto keyName   = (PCWSTR)((PBYTE)info + info->KeyNameOffset);
                auto valueName = (PCWSTR)((PBYTE)info + info->ValueNameOffset);

                std::wstring data = std::wstring(keyName) + L"\\" + valueName +
                    L" | " + RegistryTypeName(info->DataType) +
                    L" | Size: " + std::to_wstring(info->DataSize) +
                    L" | Data: " + RegistryValue(info);

                record.Data = WStringToString(data);
                break;
            }
        }

        _Repository->Insert(record);
        buffer += header->Size;
        size   -= header->Size;
    }
}

/** @brief Converts an NT device path (\\Device\\HarddiskVolumeX\\...) to its DOS drive form. */
std::wstring EventProcessor::GetDosNameFromNTName(PCWSTR path)
{
    if (path[0] != L'\\')
    {
        return path;
    }

    static std::unordered_map<std::wstring, std::wstring> map;

    if (map.empty())
    {
        auto drives = GetLogicalDrives();
        int c = 0;
        WCHAR root[] = L"X:";
        WCHAR target[128];

        while (drives)
        {
            if (drives & 1)
            {
                root[0] = (WCHAR)(L'A' + c);
                if (QueryDosDeviceW(root, target, _countof(target)))
                {
                    map.insert({target, root});
                }
            }
            drives >>= 1;
            c++;
        }
    }

    auto pos = wcschr(path + 1, L'\\');

    if (pos == nullptr)
    {
        return path;
    }

    pos = wcschr(pos + 1, L'\\');

    if (pos == nullptr)
    {
        return path;
    }

    std::wstring ntname(path, pos - path);

    if (auto it = map.find(ntname); it != map.end())
    {
        return it->second + std::wstring(pos);
    }

    return path;
}

/** @brief UTF-16 to UTF-8 conversion helper. */
std::string EventProcessor::WStringToString(const std::wstring& wstr)
{
    auto size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, nullptr, 0, nullptr, nullptr);
    auto result = std::string(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, result.data(), size, nullptr, nullptr);
    return result;
}

/** @brief Decodes the value payload of a RegistrySetValue event according to its REG_* type. */
std::wstring EventProcessor::RegistryValue(const RegistrySetValueInfo* info)
{
    auto data = (const BYTE*)info + info->DataOffset;
    // The driver caps the copied payload at ProvidedDataSize bytes (DataSize is the real size).
    DWORD size = info->ProvidedDataSize;

    switch (info->DataType)
    {
        case REG_NONE:
        {
            return L"(none)";
        }
        case REG_SZ:
        case REG_EXPAND_SZ:
        case REG_LINK:
        {
            std::wstring value((PCWSTR)data, size / sizeof(WCHAR));

            if (auto pos = value.find(L'\0'); pos != std::wstring::npos)
                value.resize(pos);

            return value;
        }
        case REG_MULTI_SZ:
        {
            std::wstring value((PCWSTR)data, size / sizeof(WCHAR));

            while (!value.empty() && value.back() == L'\0')
                value.pop_back();

            for (auto& c : value)
            {
                if (c == L'\0')
                    c = L';';
            }

            return value;
        }
        case REG_DWORD:
        case REG_DWORD_BIG_ENDIAN:
        {
            if (size < sizeof(DWORD))
                break;

            DWORD value = *(const DWORD*)data;

            if (info->DataType == REG_DWORD_BIG_ENDIAN)
                value = _byteswap_ulong(value);

            std::wstringstream ss;
            ss << L"0x" << std::hex << std::uppercase << std::setfill(L'0')
               << std::setw(8) << value
               << L" (" << std::dec << value << L")";

            return ss.str();
        }
        case REG_QWORD:
        {
            if (size < sizeof(ULONGLONG))
                break;

            ULONGLONG value = *(const ULONGLONG*)data;

            std::wstringstream ss;
            ss << L"0x" << std::hex << std::uppercase << std::setfill(L'0')
               << std::setw(16) << value
               << L" (" << std::dec << value << L")";

            return ss.str();
        }
        case REG_BINARY:
        case REG_RESOURCE_LIST:
        case REG_FULL_RESOURCE_DESCRIPTOR:
        case REG_RESOURCE_REQUIREMENTS_LIST:
        default:
        {
            break;
        }
    }

    return ToBinary(data, size);
}

/** @brief Maps a REG_* data type constant to its symbolic name. */
std::wstring EventProcessor::RegistryTypeName(ULONG type)
{
    switch (type)
    {
        case REG_NONE:                       return L"REG_NONE";
        case REG_SZ:                         return L"REG_SZ";
        case REG_EXPAND_SZ:                  return L"REG_EXPAND_SZ";
        case REG_BINARY:                     return L"REG_BINARY";
        case REG_DWORD:                      return L"REG_DWORD";
        case REG_DWORD_BIG_ENDIAN:           return L"REG_DWORD_BIG_ENDIAN";
        case REG_LINK:                       return L"REG_LINK";
        case REG_MULTI_SZ:                   return L"REG_MULTI_SZ";
        case REG_RESOURCE_LIST:              return L"REG_RESOURCE_LIST";
        case REG_FULL_RESOURCE_DESCRIPTOR:   return L"REG_FULL_RESOURCE_DESCRIPTOR";
        case REG_RESOURCE_REQUIREMENTS_LIST: return L"REG_RESOURCE_REQUIREMENTS_LIST";
        case REG_QWORD:                      return L"REG_QWORD";
        default:                             return L"REG_UNKNOWN(" + std::to_wstring(type) + L")";
    }
}

/** @brief Formats a raw byte buffer as a space-separated hex string. */
std::wstring EventProcessor::ToBinary(const BYTE* buffer, DWORD size)
{
    std::wstringstream ss;
    ss << std::hex << std::uppercase << std::setfill(L'0');

    for (DWORD i = 0; i < size; i++)
    {
        if (i > 0)
            ss << L' ';

        ss << std::setw(2) << static_cast<unsigned int>(buffer[i]);
    }

    return ss.str();
}
