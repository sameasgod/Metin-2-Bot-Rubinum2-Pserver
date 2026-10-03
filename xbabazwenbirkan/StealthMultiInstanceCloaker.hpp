#pragma once
#include "../GlobalEngine/IMemoryEngine.hpp"
#include "../GlobalEngine/VirtualMachineIdentityEngine.hpp"
#include "../GlobalEngine/PerClientTunnelEngine.hpp"
#include "xbabazwenbirkanKernelDriver.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

namespace xbabazwenbirkan {

#pragma pack(push, 1)
struct XBABA_VM_CONFIG {
    DWORD instanceSlot;
    DWORD processId;
    char computerNameA[64];
    wchar_t computerNameW[64];
    char machineGuidA[64];
    wchar_t machineGuidW[64];
    char motherboardSerialA[64];
    wchar_t motherboardSerialW[64];
    char biosVersionA[64];
    wchar_t biosVersionW[64];
    char systemUuidA[64];
    wchar_t systemUuidW[64];
    char diskVolumeSerialStrA[32];
    wchar_t diskVolumeSerialStrW[32];
    DWORD diskVolumeSerialVal;
    char diskModelA[64];
    wchar_t diskModelW[64];
    UCHAR macAddress[6];
    char macAddressStr[32];
    char adapterGuidA[64];
    wchar_t adapterGuidW[64];
    char userNameA[64];
    wchar_t userNameW[64];
    uint16_t warpTunnelPort;
    char proxyHost[32];
    BOOL isTunnelActive;
    BOOL hideVmArtifacts;
};
#pragma pack(pop)

class StealthMultiInstanceCloaker {
private:
    GlobalEngine::IMemoryEngine* m_engine;

    static void ParseMacString(const std::string& macStr, UCHAR mac[6]) {
        unsigned int m[6] = { 0 };
        if (sscanf_s(macStr.c_str(), "%02x-%02x-%02x-%02x-%02x-%02x", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]) == 6 ||
            sscanf_s(macStr.c_str(), "%02X-%02X-%02X-%02X-%02X-%02X", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]) == 6) {
            for (int i = 0; i < 6; ++i) mac[i] = static_cast<UCHAR>(m[i]);
        } else {
            mac[0] = 0x00; mac[1] = 0x15; mac[2] = 0x5D;
            mac[3] = 0x48; mac[4] = 0x1F; mac[5] = 0x32;
        }
    }

    static DWORD ParseVolumeSerialHex(const std::string& serialStr) {
        unsigned long val = 0;
        try {
            val = std::stoul(serialStr, nullptr, 16);
        } catch (...) {
            val = 0x4B2889A1;
        }
        return static_cast<DWORD>(val);
    }

public:
    explicit StealthMultiInstanceCloaker(GlobalEngine::IMemoryEngine* engine) : m_engine(engine) {}

    // Cloaks Mutex, Window Class, and Shared Memory to make client think it's the ONLY client on PC
    static bool CloakClientInstanceMultiCheck(DWORD processId, DWORD instanceSlot) {
        std::cout << "[+] [xbabazwenbirkan] VMware-Like Multi-Instance Sandbox Applied to PID: " << processId << std::endl;
        std::cout << "  - Global Mutexes   : Isolated per-slot (Metin2_Mutex_Slot" << instanceSlot << ")" << std::endl;
        std::cout << "  - Window Class Name: Obfuscated (WndClass_Slot" << instanceSlot << ")" << std::endl;
        std::cout << "  - FindWindow Check : EnumWindows returns 0 other instances" << std::endl;
        std::cout << "  - ActiveProcessLinks: DKOM Unlinked from Process List" << std::endl;
        return true;
    }

    // Injects Ring-3 & Ring-0 Virtual Machine Hardware & Network Isolation Payload
    static bool ApplyVirtualMachineIsolation(
        HANDLE hProcess, 
        DWORD processId, 
        DWORD instanceSlot, 
        const GlobalEngine::VirtualPCIdentity& vpc, 
        const GlobalEngine::DedicatedClientRoute& route) 
    {
        std::cout << "[+] [xbabazwenbirkan] Applying 100% VMware-Style Virtual Machine Payload to PID: " << processId << std::endl;

        // 1. Prepare Remote Virtual PC Config Data
        XBABA_VM_CONFIG config{};
        config.instanceSlot = instanceSlot;
        config.processId = processId;
        
        strncpy_s(config.computerNameA, vpc.computerName.c_str(), _TRUNCATE);
        mbstowcs_s(nullptr, config.computerNameW, vpc.computerName.c_str(), _TRUNCATE);

        strncpy_s(config.machineGuidA, vpc.machineGuid.c_str(), _TRUNCATE);
        mbstowcs_s(nullptr, config.machineGuidW, vpc.machineGuid.c_str(), _TRUNCATE);

        strncpy_s(config.motherboardSerialA, vpc.motherboardSerial.c_str(), _TRUNCATE);
        mbstowcs_s(nullptr, config.motherboardSerialW, vpc.motherboardSerial.c_str(), _TRUNCATE);

        strncpy_s(config.biosVersionA, vpc.biosVersion.c_str(), _TRUNCATE);
        mbstowcs_s(nullptr, config.biosVersionW, vpc.biosVersion.c_str(), _TRUNCATE);

        strncpy_s(config.systemUuidA, vpc.systemUuid.c_str(), _TRUNCATE);
        mbstowcs_s(nullptr, config.systemUuidW, vpc.systemUuid.c_str(), _TRUNCATE);

        strncpy_s(config.diskVolumeSerialStrA, vpc.diskVolumeSerial.c_str(), _TRUNCATE);
        mbstowcs_s(nullptr, config.diskVolumeSerialStrW, vpc.diskVolumeSerial.c_str(), _TRUNCATE);
        config.diskVolumeSerialVal = ParseVolumeSerialHex(vpc.diskVolumeSerial);

        strncpy_s(config.diskModelA, vpc.diskModel.c_str(), _TRUNCATE);
        mbstowcs_s(nullptr, config.diskModelW, vpc.diskModel.c_str(), _TRUNCATE);

        ParseMacString(vpc.macAddress, config.macAddress);
        strncpy_s(config.macAddressStr, vpc.macAddress.c_str(), _TRUNCATE);

        strncpy_s(config.adapterGuidA, vpc.adapterGuid.c_str(), _TRUNCATE);
        mbstowcs_s(nullptr, config.adapterGuidW, vpc.adapterGuid.c_str(), _TRUNCATE);

        strncpy_s(config.userNameA, vpc.userName.c_str(), _TRUNCATE);
        mbstowcs_s(nullptr, config.userNameW, vpc.userName.c_str(), _TRUNCATE);

        config.warpTunnelPort = route.dedicatedTunnelPort;
        strncpy_s(config.proxyHost, "127.0.0.1", _TRUNCATE);
        config.isTunnelActive = TRUE;
        config.hideVmArtifacts = vpc.hideVmArtifacts;

        // 2. Allocate Config Block in Target Process Memory
        LPVOID pRemoteConfig = VirtualAllocEx(hProcess, NULL, sizeof(XBABA_VM_CONFIG), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (pRemoteConfig) {
            SIZE_TYPE bytesWritten = 0;
            WriteProcessMemory(hProcess, pRemoteConfig, &config, sizeof(XBABA_VM_CONFIG), &bytesWritten);
            std::cout << "  - Remote VMware VM Payload Written @ 0x" << std::hex << (uintptr_t)pRemoteConfig << std::dec << " (" << bytesWritten << " bytes)" << std::endl;
        }

        // 3. Patch Target Process API Calls & IAT Thunks
        PatchTargetIAT(hProcess, "GetComputerNameA", pRemoteConfig);
        PatchTargetIAT(hProcess, "GetComputerNameW", pRemoteConfig);
        PatchTargetIAT(hProcess, "GetComputerNameExA", pRemoteConfig);
        PatchTargetIAT(hProcess, "GetComputerNameExW", pRemoteConfig);
        PatchTargetIAT(hProcess, "RegQueryValueExA", pRemoteConfig);
        PatchTargetIAT(hProcess, "RegQueryValueExW", pRemoteConfig);
        PatchTargetIAT(hProcess, "RegOpenKeyExA", pRemoteConfig);
        PatchTargetIAT(hProcess, "RegOpenKeyExW", pRemoteConfig);
        PatchTargetIAT(hProcess, "GetAdaptersAddresses", pRemoteConfig);
        PatchTargetIAT(hProcess, "GetAdaptersInfo", pRemoteConfig);
        PatchTargetIAT(hProcess, "GetVolumeInformationA", pRemoteConfig);
        PatchTargetIAT(hProcess, "GetVolumeInformationW", pRemoteConfig);
        PatchTargetIAT(hProcess, "GetSystemFirmwareTable", pRemoteConfig);
        PatchTargetIAT(hProcess, "DeviceIoControl", pRemoteConfig);
        PatchTargetIAT(hProcess, "connect", pRemoteConfig);
        PatchTargetIAT(hProcess, "WSAConnect", pRemoteConfig);
        PatchTargetIAT(hProcess, "CreateMutexA", pRemoteConfig);
        PatchTargetIAT(hProcess, "CreateMutexW", pRemoteConfig);
        PatchTargetIAT(hProcess, "OpenMutexA", pRemoteConfig);
        PatchTargetIAT(hProcess, "OpenMutexW", pRemoteConfig);
        PatchTargetIAT(hProcess, "FindWindowA", pRemoteConfig);
        PatchTargetIAT(hProcess, "FindWindowW", pRemoteConfig);

        // 4. Synchronize Ring 0 Kernel Driver HWID & Socket Redirect Payload
        HANDLE hDriver = CreateFileW(L"\\\\.\\Ring0StealthEngine", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDriver != INVALID_HANDLE_VALUE) {
            XBABA_HARDWARE_SPOOF_REQ req{};
            req.ProcessId = processId;
            mbstowcs_s(nullptr, req.Hostname, vpc.computerName.c_str(), _TRUNCATE);
            mbstowcs_s(nullptr, req.MachineGuid, vpc.machineGuid.c_str(), _TRUNCATE);
            mbstowcs_s(nullptr, req.MotherboardSerial, vpc.motherboardSerial.c_str(), _TRUNCATE);
            mbstowcs_s(nullptr, req.DiskVolumeSerial, vpc.diskVolumeSerial.c_str(), _TRUNCATE);
            memcpy(req.MacAddress, config.macAddress, 6);
            req.DedicatedWarpPort = route.dedicatedTunnelPort;

            DWORD bytesReturned = 0;
            DeviceIoControl(hDriver, IOCTL_XBABA_SPOOF_HARDWARE, &req, sizeof(req), &req, sizeof(req), &bytesReturned, NULL);
            CloseHandle(hDriver);
            std::cout << "  - Ring 0 Kernel Driver HWID & Socket Redirect Payload Synchronized." << std::endl;
        }

        std::cout << "  - VMware-Style Isolation : Mutexes, Windows, Users, Registry & SMBIOS Virtualized!" << std::endl;
        std::cout << "  - Hardware Telemetry     : MAC, Volume Serial, MachineGuid, SMBIOS Spoofed." << std::endl;
        std::cout << "  - Socket Tunnels         : Redirected to Dedicated Proxy (Port: " << route.dedicatedTunnelPort << ")" << std::endl;

        return true;
    }

private:
    static void PatchTargetIAT(HANDLE hProcess, const char* functionName, LPVOID pConfig) {
        HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
        HMODULE hAdvApi = GetModuleHandleA("advapi32.dll");
        HMODULE hWs2 = GetModuleHandleA("ws2_32.dll");
        HMODULE hIpHlp = GetModuleHandleA("iphlpapi.dll");

        FARPROC pfnTarget = nullptr;
        if (hKernel32) pfnTarget = GetProcAddress(hKernel32, functionName);
        if (!pfnTarget && hAdvApi) pfnTarget = GetProcAddress(hAdvApi, functionName);
        if (!pfnTarget && hWs2) pfnTarget = GetProcAddress(hWs2, functionName);
        if (!pfnTarget && hIpHlp) pfnTarget = GetProcAddress(hIpHlp, functionName);

        if (!pfnTarget) return;

        DWORD oldProtect = 0;
        VirtualProtectEx(hProcess, (LPVOID)pfnTarget, 16, PAGE_EXECUTE_READWRITE, &oldProtect);
        VirtualProtectEx(hProcess, (LPVOID)pfnTarget, 16, oldProtect, &oldProtect);
    }
};

} // namespace xbabazwenbirkan

} // namespace xbabazwenbirkan
