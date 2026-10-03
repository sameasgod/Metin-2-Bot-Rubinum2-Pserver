#pragma once
#include <ntddk.h>

#ifdef __cplusplus
extern "C" {
#endif

// Kernel stealth & DKOM functions
NTSTATUS StealthHideProcess(ULONG ProcessId);
NTSTATUS StealthUnlinkDriver(PDRIVER_OBJECT DriverObject);
NTSTATUS StealthZeroPEHeader(PVOID BaseAddress, ULONG Size);
NTSTATUS StealthCleanPiDDDB(UNICODE_STRING DriverName);

#ifdef __cplusplus
}
#endif
