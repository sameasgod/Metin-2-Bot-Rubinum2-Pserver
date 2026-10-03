#pragma once

#ifdef _KERNEL_MODE
#include <ntddk.h>
#else
#include <windows.h>
#include <winioctl.h>
#endif

// ============================================================================
// xbabazwenbirkan Ring 0 Kernel Driver IOCTL & Stealth Control Codes
// ============================================================================

#define FILE_DEVICE_XBABA_KERNEL 0x00009999

#define IOCTL_XBABA_SPAWN_ISOLATED   CTL_CODE(FILE_DEVICE_XBABA_KERNEL, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_XBABA_SPOOF_HARDWARE   CTL_CODE(FILE_DEVICE_XBABA_KERNEL, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_XBABA_DKOM_UNLINK      CTL_CODE(FILE_DEVICE_XBABA_KERNEL, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_XBABA_WSK_REDIRECT     CTL_CODE(FILE_DEVICE_XBABA_KERNEL, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_XBABA_RING0_INPUT      CTL_CODE(FILE_DEVICE_XBABA_KERNEL, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)

#pragma pack(push, 1)

typedef struct _XBABA_HARDWARE_SPOOF_REQ {
    ULONG ProcessId;
    WCHAR Hostname[64];
    WCHAR MachineGuid[64];
    WCHAR MotherboardSerial[64];
    WCHAR DiskVolumeSerial[32];
    UCHAR MacAddress[6];
    USHORT DedicatedWarpPort;
} XBABA_HARDWARE_SPOOF_REQ, *PXBABA_HARDWARE_SPOOF_REQ;

#pragma pack(pop)
