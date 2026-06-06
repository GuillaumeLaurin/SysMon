#include "Common.h"
#include "Public.h"
#include "Device.h"
#include "Queue.h"

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
    NT_CHECK_RETURN(status);

    LOG_INFO("DeviceCreate, device created successfully");
    return STATUS_SUCCESS;
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

VOID OnProcessNotify(
    _Inout_     PEPROCESS               Process,
    _In_        HANDLE                  ProcessId,
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO  CreateInfo
)
{
    if (CreateInfo)
    {
        USHORT allocSize = sizeof(FullItem);
        USHORT commandLineSize = 0;

        if (CreateInfo->CommandLine)
        {
            commandLineSize = CreateInfo->CommandLine->Length;
            allocSize += commandLineSize;
        }

        auto info = (FullItem*)ExAllocatePool2(
            POOL_FLAG_PAGED, allocSize, DRIVER_TAG);

        if (info == nullptr)
        {
            LOG_ERROR("failed allocation\n");
            return;
        }

        auto& item = info->Data.ProcessCreate;
        KeQuerySystemTime(&item.Time);
        item.Type               = ItemType::ProcessCreate;
        item.Size               = sizeof(ProcessCreateInfo) + commandLineSize;
        item.ProcessId          = HandleToUlong(ProcessId);
        item.ParentProcessId    = HandleToULong(CreateInfo->CreatingThreadId.UniqueProcess);
        item.CreatingThreadId   = HandleToULong(CreateInfo->CreatingThreadId.UniqueThread);

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
        auto info = (FullItem*)ExAllocatePool2(POOL_FLAG_PAGED,
            sizeof(FullItem), DRIVER_TAG);

        if (info == nullptr)
        {
            LOG_ERROR("failed allocation\n");
            return;
        }

        auto& item = info->Data.ProcessExit;
        KeQuerySystemTimePrecise(&item.Time);
        item.Type       = ItemType::ProcessExit;
        item.Size       = sizeof(ProcessExitInfo);
        item.ProcessId  = HandleToULong(ProcessId);
        item.ExitCode   = PsGetProcessExitStatus(Process);

        g_State.AddItem(&info->Entry);
    }
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
    PDEVICE_OBJECT  deviceObject    = NULL;
    UNICODE_STRING  deviceName;
    UNICODE_STRING  symLink;
    BOOLEAN         symLinkCreated  = FALSE;

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
    
    LOG_INFO("DeviceCreated, success (%wZ)", &deviceName);
    return STATUS_SUCCESS;

Cleanup:
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

VOID OnProcessNotify(
    _Inout_     PEPROCESS Process,
    _In_        HANDLE ProcessId, 
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
    if (CreateInfo)
    {
        USHORT allocSize = sizeof(FullItem);
        USHORT commandLineSize = 0;

        if (CreateInfo->CommandLine)
        {
            commandLineSize = CreateInfo->CommandLine->Length;
            allocSize += commandLineSize;
        }

        auto info = (FullItem*)ExAllocatePool2(
            POOL_FLAG_PAGED, allocSize, DRIVER_TAG);
        
        if (info == nullptr)
        {
            LOG_ERROR("failed allocation\n");
            return;
        }

        auto& item = info->Data.ProcessCreate;
        KeQuerySystemTime(&item.Time);
        item.Type = ItemType::ProcessCreate;
        item.Size = sizeof(ProcessCreateInfo) + commandLineSize;
        item.ProcessId = HandleToUlong(ProcessId);
        item.ParentProcessId = HandleToULong(
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
        auto info = (FullItem*)ExAllocatePool2(POOL_FLAG_PAGED, 
            sizeof(FullItem), DRIVER_TAG);
        
        if (info == nullptr)
        {
            LOG_ERROR("failed allocation\n");
            return;
        }

        auto& item = info->Data.ProcessExit;
        KeQuerySystemTimePrecise(&item.Time);
        item.Type = ItemType::ProcessExit;
        item.Size = sizeof(ProcessExitInfo);
        item.ProcessId = HandleToULong(ProcessId);
        item.ExitCode = PsGetProcessExitStatus(Process);

        g_State.AddItem(&info->Entry);
    }
}

#endif // USE_KMDF