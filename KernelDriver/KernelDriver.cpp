#include "KernelDriver.h"
#include "StealthDKOM.h"
#include "Ring0Input.h"

UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\Ring0StealthEngine");
UNICODE_STRING SymbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\Ring0StealthEngine");
PDEVICE_OBJECT g_DeviceObject = NULL;

extern "C" {
    NTSTATUS NTAPI MmCopyVirtualMemory(
        PEPROCESS SourceProcess,
        PVOID SourceAddress,
        PEPROCESS TargetProcess,
        PVOID TargetAddress,
        SIZE_TYPE BufferSize,
        KPROCESSOR_MODE PreviousMode,
        PSIZE_TYPE ReturnSize
    );

    NTSTATUS NTAPI PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
}

NTSTATUS ReadProcessMemoryKernel(ULONG ProcessId, ULONGLONG SourceAddress, PVOID TargetBuffer, SIZE_TYPE Size) {
    PEPROCESS TargetProcess = NULL;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)ProcessId, &TargetProcess);
    if (!NT_SUCCESS(status) || !TargetProcess) {
        return status;
    }

    SIZE_TYPE bytesCopied = 0;
    status = MmCopyVirtualMemory(
        TargetProcess,
        (PVOID)SourceAddress,
        PsGetCurrentProcess(),
        TargetBuffer,
        Size,
        KernelMode,
        &bytesCopied
    );

    ObDereferenceObject(TargetProcess);
    return status;
}

NTSTATUS WriteProcessMemoryKernel(ULONG ProcessId, ULONGLONG TargetAddress, PVOID SourceBuffer, SIZE_TYPE Size) {
    PEPROCESS TargetProcess = NULL;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)ProcessId, &TargetProcess);
    if (!NT_SUCCESS(status) || !TargetProcess) {
        return status;
    }

    SIZE_TYPE bytesCopied = 0;
    status = MmCopyVirtualMemory(
        PsGetCurrentProcess(),
        SourceBuffer,
        TargetProcess,
        (PVOID)TargetAddress,
        Size,
        KernelMode,
        &bytesCopied
    );

    ObDereferenceObject(TargetProcess);
    return status;
}

NTSTATUS IoControlHandler(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG bytesReturned = 0;

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;
    PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG inputLength = stack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outputLength = stack->Parameters.DeviceIoControl.OutputBufferLength;

    switch (controlCode) {
    case IOCTL_RING0_READ_MEMORY: {
        if (inputLength >= sizeof(RING0_MEMORY_REQUEST) && buffer) {
            PRING0_MEMORY_REQUEST req = (PRING0_MEMORY_REQUEST)buffer;
            status = ReadProcessMemoryKernel(req->ProcessId, req->SourceAddress, (PVOID)req->TargetAddress, (SIZE_TYPE)req->Size);
            bytesReturned = sizeof(RING0_MEMORY_REQUEST);
        } else {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }
    case IOCTL_RING0_WRITE_MEMORY: {
        if (inputLength >= sizeof(RING0_MEMORY_REQUEST) && buffer) {
            PRING0_MEMORY_REQUEST req = (PRING0_MEMORY_REQUEST)buffer;
            status = WriteProcessMemoryKernel(req->ProcessId, req->TargetAddress, (PVOID)req->SourceAddress, (SIZE_TYPE)req->Size);
            bytesReturned = sizeof(RING0_MEMORY_REQUEST);
        } else {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }
    case IOCTL_RING0_HIDE_PROCESS: {
        if (inputLength >= sizeof(RING0_HIDE_PROCESS_REQUEST) && buffer) {
            PRING0_HIDE_PROCESS_REQUEST req = (PRING0_HIDE_PROCESS_REQUEST)buffer;
            status = StealthHideProcess(req->ProcessId);
            bytesReturned = sizeof(RING0_HIDE_PROCESS_REQUEST);
        } else {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }
    case IOCTL_RING0_INJECT_INPUT:
    case IOCTL_XBABA_RING0_INPUT: {
        if (inputLength >= sizeof(RING0_INPUT_REQUEST) && buffer) {
            PRING0_INPUT_REQUEST req = (PRING0_INPUT_REQUEST)buffer;
            status = Ring0InjectInput(req);
            bytesReturned = sizeof(RING0_INPUT_REQUEST);
        } else {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }
    case IOCTL_XBABA_SPOOF_HARDWARE:
    case IOCTL_RING0_SPOOF_HWID: {
        if (inputLength >= sizeof(XBABA_HARDWARE_SPOOF_REQ) && buffer) {
            PXBABA_HARDWARE_SPOOF_REQ req = (PXBABA_HARDWARE_SPOOF_REQ)buffer;
            if (req->ProcessId != 0) {
                // Apply Ring 0 DKOM process unlinking automatically on HWID spoof sync
                StealthHideProcess(req->ProcessId);
            }
            status = STATUS_SUCCESS;
            bytesReturned = sizeof(XBABA_HARDWARE_SPOOF_REQ);
        } else {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }
    case IOCTL_XBABA_DKOM_UNLINK: {
        if (inputLength >= sizeof(ULONG) && buffer) {
            ULONG pid = *(PULONG)buffer;
            status = StealthHideProcess(pid);
            bytesReturned = sizeof(ULONG);
        } else {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = bytesReturned;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS CreateCloseHandler(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

void DriverUnload(PDRIVER_OBJECT DriverObject) {
    IoDeleteSymbolicLink(&SymbolicLink);
    if (DriverObject->DeviceObject) {
        IoDeleteDevice(DriverObject->DeviceObject);
    }
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status = STATUS_SUCCESS;

    status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_RING0_STEALTH, 0, FALSE, &g_DeviceObject);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = IoCreateSymbolicLink(&SymbolicLink, &DeviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(g_DeviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCloseHandler;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateCloseHandler;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControlHandler;
    DriverObject->DriverUnload = DriverUnload;

    // Stealth initialization: Unlink driver from PsLoadedModuleList
    StealthUnlinkDriver(DriverObject);
    Ring0InputInitialize();

    return STATUS_SUCCESS;
}
