#include "Common.h"
#include "Public.h"
#include "Device.h"
#include "Queue.h"

/**
 * @file Device.cpp
 * @brief Implements the process/thread/image notification callbacks and the
 *        device creation/deletion/dispatch routines declared in Device.h.
 */

/**
 * @brief Captures process creation/exit events and pushes them into the
 *        global event list. New processes are also tracked so a thread
 *        created right after by the parent is not flagged as remote.
 */
VOID
OnProcessNotify(
    _In_        PEPROCESS Process,
    _In_        HANDLE ProcessId, 
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
    if (CreateInfo)
    {
        do
        {
            auto process = (ProcessItem*)ALLOC_PAGED(sizeof(ProcessItem));

            if (process == nullptr)
            {
                LOG_ERROR("failed allocation");
                break;
            }

            process->Data.ProcessId = HandleToUlong(ProcessId);

            if (!g_State.AddNewProcess(&process->Entry))
            {
                LOG_WARN("New process created, no room to store");
                // we need to delete reference to memory 
                SAFE_FREE(process);
            } else 
            {   
                LOG_INFO("New process added: %u", process->Data.ProcessId);
            }
        } while (0);
        
        ULONG  allocSize = sizeof(FullItem<ProcessCreateInfo>);
        USHORT commandLineSize = 0;

        if (CreateInfo->CommandLine)
        {
            commandLineSize = CreateInfo->CommandLine->Length;
            allocSize += commandLineSize;
        }

        auto info = (FullItem<ProcessCreateInfo>*)ALLOC_PAGED(allocSize);
        
        if (info == nullptr)
        {
            LOG_ERROR("failed allocation");
            return;
        }

        auto& item = info->Data;
        KeQuerySystemTime(&item.Time);
        item.Type = ItemType::ProcessCreate;
        item.Size = sizeof(ProcessCreateInfo) + commandLineSize;
        item.ProcessId = HandleToUlong(ProcessId);
        item.ParentProcessId = HandleToULong(CreateInfo->ParentProcessId);
        item.CreatingProcessId = HandleToULong(
            CreateInfo->CreatingThreadId.UniqueProcess);
        item.CreatingThreadId = HandleToULong(
            CreateInfo->CreatingThreadId.UniqueThread);

        if (commandLineSize > 0)
        {
            memcpy(item.CommandLine, CreateInfo->CommandLine->Buffer, commandLineSize);
            item.CommandLineLength = commandLineSize / sizeof(WCHAR);
        }
        else
        {
            item.CommandLineLength = 0;
        }

        g_State.AddItem(&info->Entry);
    }
    else 
    {
        auto info = (FullItem<ProcessExitInfo>*)ALLOC_PAGED(sizeof(FullItem<ProcessExitInfo>));
        
        if (info == nullptr)
        {
            LOG_ERROR("failed allocation");
            return;
        }

        auto& item = info->Data;
        KeQuerySystemTimePrecise(&item.Time);
        item.Type = ItemType::ProcessExit;
        item.Size = sizeof(ProcessExitInfo);
        item.ProcessId = HandleToULong(ProcessId);
        item.ExitCode = PsGetProcessExitStatus(Process);

        g_State.AddItem(&info->Entry);
    }
}

/**
 * @brief Captures thread creation/exit events. A creation occurring in a
 *        process different from the caller's (and not tracked as a brand-new
 *        process) is additionally reported as a RemoteThread event.
 */
VOID
OnThreadNotify(
    _In_ HANDLE  ProcessId,
    _In_ HANDLE  ThreadId,
    _In_ BOOLEAN Create
)
{
    do 
    {
        if (Create)
        {
            bool remote = PsGetCurrentProcessId() != ProcessId
                && PsInitialSystemProcess != PsGetCurrentProcess()
                && PsGetProcessId(PsInitialSystemProcess) != ProcessId;
        
            if (remote)
            {
                if (g_State.RemoveProcess(ProcessId))
                {
                    // ignore
                }
                else
                {
                    // Remote thread
                    auto size = sizeof(FullItem<RemoteThread>);
                    auto info = (FullItem<RemoteThread>*)ALLOC_PAGED(size);

                    if (info == nullptr)
                    {
                        LOG_ERROR("failed allocation");
                        break;
                    }

                    auto& item = info->Data;
                    KeQuerySystemTime(&item.Time);
                    item.Size = sizeof(RemoteThread);
                    item.Type = ItemType::RemoteThread;
                    item.CreatorProcessId = HandleToULong(PsGetCurrentProcessId());
                    item.CreatorThreadId = HandleToULong(PsGetCurrentThreadId());
                    item.ProcessId = HandleToULong(ProcessId);
                    item.ThreadId = HandleToULong(ThreadId);

                    LOG_INFO("Remote thread detected. (PID: %u, TID: %u) -> (PID: %u, TID: %u)",
                        item.CreatorProcessId, item.CreatorThreadId,
                        item.ProcessId, item.ThreadId);
                    
                    g_State.AddItem(&info->Entry);
                }
            }
        }
    } while (0);
    

    auto size = Create ? sizeof(FullItem<ThreadCreateInfo>) 
        : sizeof(FullItem<ThreadExitInfo>);
    auto info = (FullItem<ThreadExitInfo>*)ALLOC_PAGED(size);
    
    if (info == nullptr)
    {
        LOG_ERROR("failed allocation");
        return;
    }
    
    auto& item = info->Data;
    KeQuerySystemTime(&item.Time);
    item.Size = Create ? sizeof(ThreadCreateInfo) : sizeof(ThreadExitInfo);
    item.Type = Create ? ItemType::ThreadCreate : ItemType::ThreadExit;
    item.ProcessId = HandleToULong(ProcessId);
    item.ThreadId = HandleToULong(ThreadId);

    if (!Create)
    {
        PETHREAD thread;
        item.ExitCode = STATUS_SUCCESS;

        if (NT_SUCCESS(PsLookupThreadByThreadId(ThreadId, &thread)))
        {
            item.ExitCode = PsGetThreadExitStatus(thread);
            ObDereferenceObject(thread);
        }
    }

    g_State.AddItem(&info->Entry);
}

/**
 * @brief Captures image-load events (EXE/DLL mappings) with the normalized
 *        file path when available. System images (null ProcessId) are ignored.
 */
VOID OnImageNotify(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_ HANDLE              ProcessId,
    _In_ PIMAGE_INFO         ImageInfo
)
{
    if (ProcessId == nullptr)
    {
        // system image, ignore
        return;
    }

    auto size = sizeof(FullItem<ImageLoadInfo>);
    auto info = (FullItem<ImageLoadInfo>*)ALLOC_PAGED(size);
    
    if (info == nullptr)
    {
        LOG_ERROR("failed allocation");
        return;
    }

    auto& item = info->Data;
    KeQuerySystemTime(&item.Time);
    item.Size = sizeof(item);
    item.Type = ItemType::ImageLoad;
    item.ProcessId = HandleToULong(ProcessId);
    item.ImageSize = (ULONG)ImageInfo->ImageSize;
    item.LoadAddress = (ULONG64)ImageInfo->ImageBase;
    
    item.ImageFileName[0] = 0;

    if (ImageInfo->ExtendedInfoPresent)
    {
        auto exInfo = CONTAINING_RECORD(ImageInfo, IMAGE_INFO_EX, ImageInfo); 
        PFLT_FILE_NAME_INFORMATION nameInfo;
        if (NT_SUCCESS(FltGetFileNameInformationUnsafe(exInfo->FileObject,
        nullptr, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
        &nameInfo)))
        {
            // UNICODE_STRING buffers are not null-terminated: bounded copy
            ULONG chars = min((ULONG)(nameInfo->Name.Length / sizeof(WCHAR)),
                (ULONG)MaxImageFileSize);
            memcpy(item.ImageFileName, nameInfo->Name.Buffer, chars * sizeof(WCHAR));
            item.ImageFileName[chars] = 0;
            FltReleaseFileNameInformation(nameInfo);
        }
    }

    if (item.ImageFileName[0] == 0 && FullImageName)
    {
        ULONG chars = min((ULONG)(FullImageName->Length / sizeof(WCHAR)),
            (ULONG)MaxImageFileSize);
        memcpy(item.ImageFileName, FullImageName->Buffer, chars * sizeof(WCHAR));
        item.ImageFileName[chars] = 0;
    }

    g_State.AddHeadItem(&info->Entry);
}

/**
 * 
 */
NTSTATUS
OnRegistryNotify(
    _In_ PVOID context,
    _In_ PVOID arg1,
    _In_ PVOID arg2
)
{
    UNREFERENCED_PARAMETER(context);

    switch ((REG_NOTIFY_CLASS)(ULONG_PTR)arg1)
    {
        case RegNtPostSetValueKey:
        {
            auto args = (REG_POST_OPERATION_INFORMATION*)arg2;

            if (!NT_SUCCESS(args->Status))
                break;
            
            static const WCHAR machine[] = L"\\REGISTRY\\MACHINE\\";
            PCUNICODE_STRING name;
            LARGE_INTEGER cookie = g_State.GetCookie();

            if (NT_SUCCESS(CmCallbackGetKeyObjectIDEx(&cookie,
                    args->Object, nullptr, &name, 0)))
            {
                if (wcsncmp(name->Buffer, machine, ARRAY_SIZE(machine) - 1) == 0)
                {
                    auto preInfo = (REG_SET_VALUE_KEY_INFORMATION*)args->PreInformation;

                    NT_ASSERT(preInfo);

                    USHORT size = sizeof(RegistrySetValueInfo);
                    USHORT keyNameLen = name->Length + sizeof(WCHAR);
                    USHORT valueNameLen = preInfo->ValueName->Length + sizeof(WCHAR);\

                    // restrict copied data to 256 bytes
                    USHORT valueSize = (USHORT)min(256, preInfo->DataSize);
                    size += keyNameLen + valueNameLen + valueSize;

                    auto info = (FullItem<RegistrySetValueInfo>*)ALLOC_PAGED(size + sizeof(LIST_ENTRY));

                    if (info)
                    {
                        auto& data = info->Data;
                        KeQuerySystemTimePrecise(&data.Time);
                        data.Type = ItemType::RegistrySetValue;
                        data.Size = size;
                        data.DataType = preInfo->Type;
                        data.ProcessId = HandleToULong(PsGetCurrentProcessId());
                        data.ThreadId = HandleToULong(PsGetCurrentThreadId());
                        data.ProvidedDataSize = valueSize;
                        data.DataSize = preInfo->DataSize;

                        USHORT offset = sizeof(data);
                        data.KeyNameOffset = offset;
                        wcsncpy_s(
                            (PWSTR)((PUCHAR)&data + offset),
                            keyNameLen / sizeof(WCHAR), name->Buffer,
                            name->Length / sizeof(WCHAR)
                        );
                        offset += keyNameLen;
                        data.ValueNameOffset = offset;
                        wcsncpy_s(
                            (PWSTR)((PUCHAR)&data + offset),
                            valueNameLen / sizeof(WCHAR), preInfo->ValueName->Buffer,
                            preInfo->ValueName->Length / sizeof(WCHAR)
                        );
                        offset += valueNameLen;
                        data.DataOffset = offset;
                        memcpy((PUCHAR)&data + offset, preInfo->Data, valueSize);

                        g_State.AddItem(&info->Entry);

                        
                    }
                } else {
                    LOG_ERROR("Failed to allocate memory for registry set value\n");
                }
            }
            CmCallbackReleaseKeyObjectIDEx(name);
        }
        break;
    }

    return STATUS_SUCCESS;
}

#ifdef USE_KMDF

/**
 * @brief Creates and configures the WDF device objects
 * 
 * @param DeviceInit Initialization structure provided by the framework
 * @return NTSTATUS
 */
NTSTATUS
DeviceCreate(
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    NTSTATUS                status;
    WDFDEVICE               device;
    WDF_OBJECT_ATTRIBUTES   deviceAttributes;
    PDEVICE_CONTEXT         deviceContext;
    UNICODE_STRING          deviceName;
    UNICODE_STRING          symLink;
    UNICODE_STRING          altitude = RTL_CONSTANT_STRING(L"7657.124");
    BOOLEAN                 processNotifySet = FALSE;
    BOOLEAN                 threadNotifySet  = FALSE;
    BOOLEAN                 imageNotifySet   = FALSE;

    LOG_TRACE("DeviceCreate (KMDF)");

    RtlInitUnicodeString(&deviceName, DRIVER_DEVICE_NAME);
    status = WdfDeviceInitAssignName(DeviceInit, &deviceName);
    NT_CHECK_RETURN(status);

    WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_UNKNOWN);
    WdfDeviceInitSetExclusive(DeviceInit, FALSE);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);
    deviceAttributes.EvtCleanupCallback = EvtDeviceContextCleanup;

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    NT_CHECK_RETURN(status);

    // Initialize device content
    deviceContext           = DeviceGetContext(device);
    deviceContext->IsOpen   = FALSE;
    deviceContext->DeviceId = 0;

    // Create user-mode accessible symbolic link
    RtlInitUnicodeString(&symLink, DRIVER_SYMLINK_NAME);
    status = WdfDeviceCreateSymbolicLink(device, &symLink);
    NT_CHECK_RETURN(status);

    // Initialize the I/O queue
    status = QueueInitialize(device);
    NT_CHECK_RETURN(status);

    status = PsSetCreateProcessNotifyRoutineEx(OnProcessNotify, FALSE);
    NT_CHECK_GOTO(status, Cleanup);
    processNotifySet = TRUE;

    status = PsSetCreateThreadNotifyRoutine(OnThreadNotify);
    NT_CHECK_GOTO(status, Cleanup);
    threadNotifySet = TRUE;

    status = PsSetLoadImageNotifyRoutine(OnImageNotify);
    NT_CHECK_GOTO(status, Cleanup);
    imageNotifySet = TRUE;

    status = g_State.RegisterCallback(
        OnRegistryNotify,
        &altitude,
        WdfDriverWdmGetDriverObject(WdfGetDriver()),
        nullptr,
        nullptr);
    NT_CHECK_GOTO(status, Cleanup);

    LOG_INFO("DeviceCreate, device created successfully");
    return STATUS_SUCCESS;

Cleanup:
    if (imageNotifySet) {
        PsRemoveLoadImageNotifyRoutine(OnImageNotify);
    }
    if (threadNotifySet) {
        PsRemoveCreateThreadNotifyRoutine(OnThreadNotify);
    }
    if (processNotifySet) {
        PsSetCreateProcessNotifyRoutineEx(OnProcessNotify, TRUE);
    }
    return status;
}

/**
 * @brief KMDF cleanup callback — invoked before the device context is freed.
 */
VOID
EvtDeviceContextCleanup(
    _In_ WDFOBJECT DeviceObject
)
{
    PDEVICE_CONTEXT ctx = DeviceGetContext(DeviceObject);
    UNREFERENCED_PARAMETER(ctx);

    LOG_TRACE("EvtDeviceContextCleanup");

    // Release any resources allocated in the device context here !
}

#else // !USE_KMDF

/**
 * @brief Creates the device object and symbolic link (WDM).
 * 
 * @param DriverObject Pointer to the driver object
 * @param RegistryPath Driver registry path
 * @return NTSTATUS
 */
NTSTATUS
DeviceCreate(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    NTSTATUS        status;
    PDEVICE_OBJECT  deviceObject          = NULL;
    UNICODE_STRING  deviceName;
    UNICODE_STRING  symLink;
    UNICODE_STRING  altitude              = RTL_CONSTANT_STRING(L"7657.124");
    BOOLEAN         symLinkCreated        = FALSE;
    BOOLEAN         processNotifySet      = FALSE;
    BOOLEAN         threadNotifySet       = FALSE;

    UNREFERENCED_PARAMETER(RegistryPath);

    LOG_TRACE("DeviceCreate (WDM)");

    RtlInitUnicodeString(&deviceName, DRIVER_DEVICE_NAME);

    status = IoCreateDevice(
        DriverObject,
        0,
        &deviceName,
        FILE_DEVICE_UNKNOWN,
        0,
        TRUE,
        &deviceObject
    );
    NT_CHECK_GOTO(status, Cleanup);

    // DO_DIRECT_IO is better for own memory and cache management
    deviceObject->Flags |= DO_DIRECT_IO;

    RtlInitUnicodeString(&symLink, DRIVER_SYMLINK_NAME);
    status = IoCreateSymbolicLink(&symLink, &deviceName);
    NT_CHECK_GOTO(status, Cleanup);

    symLinkCreated = TRUE;

    status = PsSetCreateProcessNotifyRoutineEx(OnProcessNotify, FALSE);
    NT_CHECK_GOTO(status, Cleanup);

    processNotifySet = TRUE;

    status = PsSetCreateThreadNotifyRoutine(OnThreadNotify);
    NT_CHECK_GOTO(status, Cleanup);

    threadNotifySet = TRUE;

    status = PsSetLoadImageNotifyRoutine(OnImageNotify);
    NT_CHECK_GOTO(status, Cleanup);

    status = g_State.RegisterCallback(OnRegistryNotify, &altitude, DriverObject, nullptr, nullptr);
    NT_CHECK_GOTO(status, Cleanup);

    LOG_INFO("DeviceCreated, success (%wZ)", &deviceName);
    return STATUS_SUCCESS;

Cleanup:
    if (threadNotifySet) {
        PsRemoveCreateThreadNotifyRoutine(OnThreadNotify);
    }
    if (processNotifySet) {
        PsSetCreateProcessNotifyRoutineEx(OnProcessNotify, TRUE);
    }
    if (symLinkCreated) {
        IoDeleteSymbolicLink(&symLink);
    }
    if (deviceObject != NULL) {
        IoDeleteDevice(deviceObject);
    }
    return status;
}

/**
 * @brief Deletes the device object and symbolic link (WDM).
 */
VOID
DeviceDelete(
    _In_ PDEVICE_OBJECT DeviceObject
)
{
    UNICODE_STRING symLink;

    LOG_TRACE("DeviceDelete");

    RtlInitUnicodeString(&symLink, DRIVER_SYMLINK_NAME);
    IoDeleteSymbolicLink(&symLink);

    if (DeviceObject != NULL) {
        IoDeleteDevice(DeviceObject);
    }
}

/**
 * @brief Completes an IRP with the given status and information count.
 */
NTSTATUS
CompleteRequest(
    _Inout_ PIRP        Irp,
    _In_    NTSTATUS    status,
    _In_    ULONG_PTR   info) 
{
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = info;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

/**
 * @brief IRP_MJ_CREATE handler (WDM) — always succeeds, no per-open state to set up.
 */
NTSTATUS
DispatchCreate(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP        Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    LOG_TRACE("DispatchCreate");
    return CompleteRequest(Irp);
}

/**
 * @brief IRP_MJ_CLOSE handler (WDM).
 */
NTSTATUS
DispatchClose(
    _In_    PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP           Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    LOG_TRACE("DispatchClose");
    return CompleteRequest(Irp);
}

/**
 * @brief IRP_MJ_CLEANUP handler (WDM).
 */
NTSTATUS
DispatchCleanup(
    _In_    PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP           Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    LOG_TRACE("DispatchCleanup");
    return CompleteRequest(Irp);
}

#endif // USE_KMDF