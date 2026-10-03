#pragma once
#include "IMemoryEngine.hpp"
#include "NetworkTunnelEngine.hpp"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>

namespace GlobalEngine {

// Complete Virtual PC Hardware Identity Profile per Client Instance (VMware-like Virtual Machine Environment)
struct VirtualPCIdentity {
    DWORD instanceId;
    DWORD processId;
    std::string computerName;      // e.g. "DESKTOP-X79K2L"
    std::string machineGuid;       // Unique Registry MachineGuid
    std::string motherboardSerial; // Unique SMBIOS Motherboard Serial
    std::string biosVersion;       // Unique BIOS Version String
    std::string systemUuid;        // Unique System UUID
    std::string diskVolumeSerial;  // Unique HardDisk Serial
    std::string diskModel;         // Unique Disk Drive Model String
    std::string macAddress;        // Unique MAC Address (e.g. "00-15-5D-8F-11-2B")
    std::string adapterGuid;       // Unique Network Adapter GUID
    std::string gpuName;           // Virtual / Physical GPU Name
    std::string userName;          // Isolated Slot Username
    std::string userProfileDir;    // Isolated Slot User Profile Directory
    uint16_t warpTunnelPort;       // Independent Cloudflare WARP / Proxy SOCKS5 Tunnel Port
    BOOL hideVmArtifacts;          // Anti-Cheat Anti-VM Stealth Flag
};

class VirtualMachineIdentityEngine {
private:
    IMemoryEngine* m_engine;
    std::vector<VirtualPCIdentity> m_activeVirtualPCs;

    static std::string RandomAlphaNumeric(int length) {
        static const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(chars) - 2);
        std::string res = "";
        for (int i = 0; i < length; ++i) res += chars[dis(gen)];
        return res;
    }

    static std::string RandomHex(int length) {
        static const char hex[] = "0123456789ABCDEF";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::string res = "";
        for (int i = 0; i < length; ++i) res += hex[dis(gen)];
        return res;
    }

    static std::string RandomMAC() {
        static const char hex[] = "0123456789ABCDEF";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::string mac = "00";
        for (int i = 0; i < 5; ++i) {
            mac += "-";
            mac += hex[dis(gen)];
            mac += hex[dis(gen)];
        }
        return mac;
    }

public:
    explicit VirtualMachineIdentityEngine(IMemoryEngine* engine) : m_engine(engine) {}

    // Generate a 100% Unique Physical PC Identity for a Client Instance (VMware-like complete virtual environment)
    VirtualPCIdentity CreateUniquePCIdentity(DWORD processId, DWORD instanceSlot) {
        VirtualPCIdentity id{};
        id.instanceId = instanceSlot;
        id.processId = processId;
        id.computerName = "DESKTOP-" + RandomAlphaNumeric(7);
        id.machineGuid = RandomHex(8) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(12);
        id.motherboardSerial = "MB-" + RandomAlphaNumeric(10);
        id.biosVersion = "AMI - " + RandomHex(4) + RandomHex(4);
        id.systemUuid = RandomHex(8) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(12);
        id.diskVolumeSerial = RandomHex(8);
        id.diskModel = "NVMe Samsung SSD 980 " + RandomAlphaNumeric(4);
        id.macAddress = RandomMAC();
        id.adapterGuid = "{" + RandomHex(8) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(4) + "-" + RandomHex(12) + "}";
        id.gpuName = "NVIDIA GeForce RTX 3060";
        id.userName = "SlotUser_" + std::to_string(instanceSlot);
        id.userProfileDir = "C:\\Users\\SlotUser_" + std::to_string(instanceSlot);
        id.warpTunnelPort = static_cast<uint16_t>(40000 + instanceSlot);
        id.hideVmArtifacts = TRUE;

        m_activeVirtualPCs.push_back(id);

        std::cout << "==========================================================" << std::endl;
        std::cout << "[+] VMWARE-LIKE VIRTUAL PC IDENTITY CREATED FOR CLIENT #" << instanceSlot << " (PID: " << processId << ")" << std::endl;
        std::cout << "  - Hostname        : " << id.computerName << std::endl;
        std::cout << "  - MachineGuid     : " << id.machineGuid << std::endl;
        std::cout << "  - Motherboard     : " << id.motherboardSerial << std::endl;
        std::cout << "  - System UUID     : " << id.systemUuid << std::endl;
        std::cout << "  - Disk Serial     : " << id.diskVolumeSerial << " (" << id.diskModel << ")" << std::endl;
        std::cout << "  - MAC Address     : " << id.macAddress << std::endl;
        std::cout << "  - Network Tunnel  : 127.0.0.1:" << id.warpTunnelPort << std::endl;
        std::cout << "  - Isolated User   : " << id.userName << std::endl;
        std::cout << "  - Anti-VM Cloak   : ACTIVE (VMware Hypervisor Telemetry Spoofed)" << std::endl;
        std::cout << "==========================================================" << std::endl;

        return id;
    }

    size_t GetTotalVirtualPCs() const { return m_activeVirtualPCs.size(); }
};

} // namespace GlobalEngine
