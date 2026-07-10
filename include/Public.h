#pragma once

/**
 * @file Public.h
 * @brief Event-item structures shared between the kernel driver and the
 *        user-mode client: item types, per-event payloads, and the process
 *        tracking record used for remote-thread detection.
 */

/**
 * @brief Maximum number of characters stored for an image file path (excluding the terminator).
 */
CONST INT MaxImageFileSize = 300;

/**
 * @brief Discriminates the concrete payload carried by an ItemHeader.
 */
enum class ItemType : short
{
    None,
    ProcessCreate,
    ProcessExit,
    ThreadCreate,
    ThreadExit,
    ImageLoad,
    RemoteThread,
    RegistrySetValue,
};

/**
 * @brief Holds information common to all event types.
 */
struct ItemHeader
{
    ItemType      Type; /**< Concrete event type of this item */
    USHORT        Size; /**< Total size in bytes of the derived struct (including variable data) */
    LARGE_INTEGER Time; /**< System time at which the event was captured */
};

/**
 * @brief Emitted when a process exits.
 */
struct ProcessExitInfo : ItemHeader
{
    ULONG ProcessId; /**< ID of the exiting process */
    ULONG ExitCode;  /**< Process exit status */
};

/**
 * @brief Emitted when a process is created. Variable-length: the command
 *        line is stored inline right after the fixed part.
 */
struct ProcessCreateInfo : ItemHeader
{
    ULONG  ProcessId;         /**< ID of the new process */
    ULONG  ParentProcessId;   /**< ID of the parent process */
    ULONG  CreatingThreadId;  /**< Thread that issued the creation */
    ULONG  CreatingProcessId; /**< Process that issued the creation */
    USHORT CommandLineLength; /**< Command line length, in WCHARs */
    WCHAR  CommandLine[1];    /**< Inline command line (not null-terminated) */
};

/**
 * @brief Emitted when a thread is created.
 */
struct ThreadCreateInfo : ItemHeader
{
    ULONG ThreadId;  /**< ID of the new thread */
    ULONG ProcessId; /**< ID of the owning process */
};

/**
 * @brief Emitted when a thread exits.
 */
struct ThreadExitInfo : ThreadCreateInfo
{
    ULONG ExitCode; /**< Thread exit status */
};

/**
 * @brief Emitted when an executable image (EXE/DLL/driver) is mapped into a process.
 */
struct ImageLoadInfo : ItemHeader
{
    ULONG   ProcessId;   /**< ID of the process loading the image */
    ULONG   ImageSize;   /**< Size of the mapped image, in bytes */
    ULONG64 LoadAddress; /**< Base address the image was mapped at */
    WCHAR   ImageFileName[MaxImageFileSize + 1]; /**< Null-terminated image path */
};

/**
 * @brief Emitted when a thread is created in a process other than the creator's
 *        (potential code injection).
 */
struct RemoteThread : ItemHeader
{
    ULONG CreatorProcessId; /**< Process that created the remote thread */
    ULONG CreatorThreadId;  /**< Thread that created the remote thread */
    ULONG ProcessId;        /**< Target process receiving the thread */
    ULONG ThreadId;         /**< ID of the new remote thread */
};

/**
 * @brief Minimal record kept in the new-processes list for remote-thread detection.
 */
struct Process
{
    ULONG ProcessId; /**< Tracked process ID */
};

/**
 * @brief Kernel-internal wrapper combining the intrusive list node and the
 *        typed event data in a single allocation.
 */
template<typename T>
struct FullItem
{
    LIST_ENTRY Entry; /**< Intrusive list node */
    T          Data;  /**< Typed event payload */
};

using ProcessItem = FullItem<Process>;

/**
 * @brief Emitted when a registry value is written (RegNtPostSetValueKey) under
 *        HKLM. Variable-length: the null-terminated key name, null-terminated
 *        value name and raw data are stored inline after the fixed part, at
 *        the offsets below.
 */
struct RegistrySetValueInfo : ItemHeader
{
    ULONG  ProcessId;        /**< ID of the process writing the value */
    ULONG  ThreadId;         /**< ID of the thread writing the value */
    USHORT KeyNameOffset;    /**< Offset of the key name, from the beginning of the structure */
    USHORT ValueNameOffset;  /**< Offset of the value name, from the beginning of the structure */
    ULONG  DataType;         /**< Value type (REG_SZ, REG_DWORD, REG_BINARY, ...) */
    ULONG  DataSize;         /**< Actual size of the written data, in bytes */
    USHORT DataOffset;       /**< Offset of the copied data, from the beginning of the structure */
    USHORT ProvidedDataSize; /**< Number of data bytes copied inline (capped at 256) */
};
