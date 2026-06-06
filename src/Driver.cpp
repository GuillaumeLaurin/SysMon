#include "Common.h"
#include "Public.h"
#include "Device.h"
#include "Queue.h"
#include "Globals.h"

#define HARDCODED_LIMIT 10000

#ifdef USE_KMDF

EVT_WDF_DRIVER_UNLOAD EvtDriverUnload;

/**
 * @brief KMDF driver entry point. Called by FxDriverEntry after framework init.
 * 
 * @param DriverObject DRIVER_OBJECT allocated by the kernel
 * @param RegistryPath Registry path for this driver
 * @return NTSTATUS
 */
extern "C"
NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    NTSTATUS                status;
    WDF_DRIVER_CONFIG       config;
    WDF_OBJECT_ATTRIBUTES   attributes;
    
    LOG_INFO("DriverEntry - start (KMDF %d.%d)", KMDF_VERSION_MAJOR, KMDF_VERSION_MINOR);

    g_State.Init(HARDCODED_LIMIT);

    WDF_DRIVER_CONFIG_INIT(&config, EvtDriverDeviceAdd);
    config.EvtDriverUnload = EvtDriverUnload;

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        &attributes,
        &config,
        WDF_NO_HANDLE
    );

    NT_CHECK_RETURN(status);

    LOG_INFO("DriverEntry - success");
    return STATUS_SUCCESS;
}

/**
 * @brief KMDF callback - invoked for each PnP device instance.
 */
NTSTATUS
EvtDriverDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    UNREFERENCED_PARAMETER(Driver);
    LOG_TRACE("EvtDriverAddDevice");
    return DeviceCreate(DeviceInit);
}

/**
 * @brief KMDF callback - invoked when the driver is being unloaded.
 */
VOID
EvtDriverUnload(
    _In_ WDFDRIVER Driver
)
{
    UNREFERENCED_PARAMETER(Driver);
    LOG_INFO("EvtDriverUnload - driver unload");

    PsSetCreateProcessNotifyRoutineEx(OnProcessNotify, TRUE);
    PsRemoveCreateThreadNotifyRoutine(OnThreadNotify);
    PsRemoveLoadImageNotifyRoutine(OnImageNotify);

    PLIST_ENTRY entry;
    while ((entry = g_State.RemoveItem()) != nullptr)
    {
        ExFreePool(CONTAINING_RECORD(entry, FullItem, Entry));
    }

    g_State.Destroy();
}

#else // !USE_KMDF

DRIVER_UNLOAD DriverUnload;

/**
 * @brief WDM driver entry point.
 *
 * @param DriverObject  DRIVER_OBJECT allocated by the kernel
 * @param RegistryPath  Registry path for this driver
 * @return NTSTATUS
 */
extern "C"
NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    NTSTATUS status;

    LOG_INFO("DriverEntry - start (WDM)");

    status = DeviceCreate(DriverObject, RegistryPath);

    NT_CHECK_RETURN(status);

    g_State.Init(HARDCODED_LIMIT);

    DriverObject->DriverUnload = DriverUnload;

    DriverObject->MajorFunction[IRP_MJ_CREATE]         = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP]        = DispatchCleanup;
    DriverObject->MajorFunction[IRP_MJ_READ]           = DispatchRead;
    DriverObject->MajorFunction[IRP_MJ_WRITE]          = DispatchWrite;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;

    LOG_INFO("DriverEntry - success");
    return STATUS_SUCCESS;
}

/**
 * @brief WDM unload routine - clean up before the driver is removed.
 */
VOID
DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    LOG_INFO("DriverUnload - cleaning up...");

    PsSetCreateProcessNotifyRoutineEx(OnProcessNotify, TRUE);
    PsRemoveCreateThreadNotifyRoutine(OnThreadNotify);
    PsRemoveLoadImageNotifyRoutine(OnImageNotify);

    PLIST_ENTRY entry;
    while ((entry = g_State.RemoveItem()) != nullptr)
    {
        ExFreePool(CONTAINING_RECORD(entry, FullItem, Entry));
    }

    g_State.Destroy();
    DeviceDelete(DriverObject->DeviceObject);
    LOG_INFO("DriverUnload - done");
}

#endif // USE_KMDF