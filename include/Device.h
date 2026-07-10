#pragma once

/**
 * @file Device.h
 * @brief Process/thread/image notification callbacks and device
 *        creation/deletion/dispatch routines (KMDF and WDM variants).
 */

#include "Common.h"

/**
 * @brief Process create/exit notification callback
 *        (registered with PsSetCreateProcessNotifyRoutineEx).
 *        Queues a ProcessCreateInfo or ProcessExitInfo event and tracks new
 *        processes for remote-thread detection.
 *
 * @param Process    Process object being created or deleted
 * @param ProcessId  ID of the process
 * @param CreateInfo Creation details; nullptr means the process is exiting
 */
VOID OnProcessNotify(
    _In_        PEPROCESS Process,
    _In_        HANDLE                  ProcessId,
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO  CreateInfo);

/**
 * @brief Thread create/exit notification callback
 *        (registered with PsSetCreateThreadNotifyRoutine).
 *        Queues a ThreadCreateInfo/ThreadExitInfo event, plus a RemoteThread
 *        event when the creating process differs from the target process.
 *
 * @param ProcessId ID of the process owning the thread
 * @param ThreadId  ID of the thread
 * @param Create    TRUE on creation, FALSE on exit
 */
VOID OnThreadNotify(
    _In_ HANDLE  ProcessId,
    _In_ HANDLE  ThreadId,
    _In_ BOOLEAN Create
);

/**
 * @brief Image-load notification callback
 *        (registered with PsSetLoadImageNotifyRoutine).
 *        Queues an ImageLoadInfo event with the resolved image path.
 *
 * @param FullImageName Image path as provided by the kernel (may be null)
 * @param ProcessId     Process the image is mapped into (null for system images)
 * @param ImageInfo     Image details (base address, size, extended info)
 */
VOID OnImageNotify(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_ HANDLE              ProcessId,
    _In_ PIMAGE_INFO         ImageInfo
);

NTSTATUS OnRegistryNotify(
    _In_ PVOID context,
    _In_ PVOID arg1,
    _In_ PVOID arg2
);

#ifdef USE_KMDF

EVT_WDF_DRIVER_DEVICE_ADD EvtDriverDeviceAdd;

EVT_WDF_DEVICE_CONTEXT_CLEANUP EvtDeviceContextCleanup;

/**
 * @brief Creates and configures the WDF device objects
 *
 * @param DeviceInit Initialization structure provided by the framework
 * @return NTSTATUS
 */
NTSTATUS
DeviceCreate(
    _Inout_ PWDFDEVICE_INIT DeviceInit
);

#else

/**
 * @brief Creates the device object and symbolic link (WDM).
 *
 * @param DriverObject Pointer to the driver object
 * @param RegistryPath Driver registry path
 * @return NTSTATUS
 */
NTSTATUS
DeviceCreate(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    RegistryPath
);

/**
 * @brief Deletes the device object and symbolic link (WDM).
 *
 * @param DeviceObject Pointer to the DEVICE_OBJECT to remove
 */
VOID
DeviceDelete(
    _In_ PDEVICE_OBJECT DeviceObject
);

/**
 * @brief Completes an IRP with the given status and information count.
 *
 * @param Irp    IRP to complete
 * @param status Completion status
 * @param info   Value for IoStatus.Information (e.g. bytes transferred)
 * @return The completion status, for convenient tail-return from dispatch routines
 */
NTSTATUS CompleteRequest(
    _Inout_ PIRP      Irp,
    _In_    NTSTATUS  status = STATUS_SUCCESS,
    _In_    ULONG_PTR info = 0);

DRIVER_DISPATCH DispatchCreate;
DRIVER_DISPATCH DispatchClose;
DRIVER_DISPATCH DispatchCleanup;

#endif
