#pragma once

CONST INT MaxImageFileSize = 300;

enum class ItemType : short 
{
    None,
    ProcessCreate,
    ProcessExit
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

struct ItemData : ItemHeader
{
    union {
        ProcessCreateInfo ProcessCreate;
        ProcessExitInfo ProcessExit;
    };
};

struct FullItem
{   
    LIST_ENTRY Entry;
    ItemData Data;
};

