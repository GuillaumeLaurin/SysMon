#include "core/EventProcessor.hpp"

#include "Public.h"

#include <objbase.h>

#include <unordered_map>

EventProcessor::EventProcessor(
    std::shared_ptr<IDriverConnector> connector,
    std::shared_ptr<IEventRepository> repository,
    std::shared_ptr<IErrorDispatcher> dispatcher
)   : _Connector(connector), _Repository(repository), _Dispatcher(dispatcher)
{
}

EventProcessor::~EventProcessor()
{
    Stop();
}

void EventProcessor::Start() noexcept
{
    if (!_Running)
    {
        _Running = true;
        _Worker = std::thread(&EventProcessor::WorkerLoop, this);
    }
}

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
        }

        _Repository->Insert(record);
        buffer += header->Size;
        size   -= header->Size;
    }
}

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

std::string EventProcessor::WStringToString(const std::wstring& wstr)
{
    auto size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, nullptr, 0, nullptr, nullptr);
    auto result = std::string(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, result.data(), size, nullptr, nullptr);
    return result;
}
