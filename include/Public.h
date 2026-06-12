#pragma once

CONST INT MaxImageFileSize = 300;

enum class ItemType : short 
{
    None,
    ProcessCreate,
    ProcessExit,
    ThreadCreate,
    ThreadExit,
    ImageLoad,
    RemoteThread
};

/**
 * @brief Holds information common to all event types
 */
struct ItemHeader
{
    ItemType      Type;
    USHORT        Size;
    LARGE_INTEGER Time;
};

struct ProcessExitInfo : ItemHeader
{
    ULONG ProcessId;
    ULONG ExitCode;
};

struct ProcessCreateInfo : ItemHeader
{
    ULONG  ProcessId;
    ULONG  ParentProcessId;
    ULONG  CreatingThreadId;
    ULONG  CreatingProcessId;
    USHORT CommandLineLength;
    WCHAR  CommandLine[1];
};

struct ThreadCreateInfo : ItemHeader
{
    ULONG ThreadId;
    ULONG ProcessId;
};

struct ThreadExitInfo : ThreadCreateInfo
{
    ULONG ExitCode;
};

struct ImageLoadInfo : ItemHeader
{
    ULONG   ProcessId;
    ULONG   ImageSize;
    ULONG64 LoadAddress;
    WCHAR   ImageFileName[MaxImageFileSize + 1];
};

struct RemoteThread : ItemHeader
{
    ULONG         CreatorProcessId;
    ULONG         CreatorThreadId;
    ULONG         ProcessId;
    ULONG         ThreadId;
};

struct Process
{
    ULONG ProcessId;
};

template<typename T>
struct FullItem
{   
    LIST_ENTRY Entry;
    T          Data;
};

using ProcessItem = FullItem<Process>;

