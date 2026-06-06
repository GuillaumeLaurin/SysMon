#pragma once

CONST INT MaxImageFileSize = 300;

enum class ItemType : short 
{
    None,
    ProcessCreate,
    ProcessExit,
    ThreadCreate,
    ThreadExit,
    ImageLoad
};

/**
 * @brief Holds information common to all event types
 */
struct ItemHeader
{
    ItemType Type;
    USHORT Size;
    LARGE_INTEGER Time;
};

struct ProcessExitInfo : ItemHeader
{
    ULONG ProcessId;
    ULONG ExitCode;
};

struct FullProcessExitInfo
{
    LIST_ENTRY Entry;
    ProcessExitInfo Data;
};

struct ProcessCreateInfo : ItemHeader
{
    ULONG ProcessId;
    ULONG ParentProcessId;
    ULONG CreatingThreadId;
    ULONG CreatingProcessId;
    USHORT CommandLineLength;
    WCHAR CommandLine[1];
};

struct FullProcessCreateInfo 
{
    LIST_ENTRY Entry;
    ProcessCreateInfo Data;
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

struct ItemData : ItemHeader
{
    union {
        ProcessCreateInfo   ProcessCreate;
        ProcessExitInfo     ProcessExit;
        ThreadCreateInfo    ThreadCreate;
        ThreadExitInfo      ThreadExit;
        ImageLoadInfo       ImageLoad;
    };
};

struct FullItem
{   
    LIST_ENTRY Entry;
    ItemData Data;
};

