#pragma once
#include "../GlobalEngine/VirtualMachineIdentityEngine.hpp"
#include "../GlobalEngine/PerClientTunnelEngine.hpp"
#include "../GlobalEngine/KernelMemoryEngine.hpp"
#include "StealthMultiInstanceCloaker.hpp"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

namespace xbabazwenbirkan {

struct LaunchedClientInfo {
    DWORD instanceSlot;
    DWORD processId;
    std::wstring clientPath;
    GlobalEngine::VirtualPCIdentity virtualPcIdentity;
    GlobalEngine::DedicatedClientRoute tunnelRoute;
};

class LauncherEngine {
private:
    std::unique_ptr<GlobalEngine::KernelMemoryEngine> m_kernelEngine;
    GlobalEngine::VirtualMachineIdentityEngine m_vmEngine;
    GlobalEngine::PerClientTunnelEngine m_tunnelEngine;

    std::vector<LaunchedClientInfo> m_launchedClients;
    std::wstring m_selectedClientPath;

public:
    LauncherEngine() 
        : m_kernelEngine(std::make_unique<GlobalEngine::KernelMemoryEngine>()),
          m_vmEngine(m_kernelEngine.get()),
          m_tunnelEngine(m_kernelEngine.get()) {}

    void SetClientExecutablePath(const std::wstring& exePath) {
        m_selectedClientPath = exePath;
        std::wcout << L"[xbabazwenbirkan] Target Client Path Set: " << m_selectedClientPath << std::endl;
    }

    // Launch a new isolated client instance with individual IP and Virtual PC Identity
    bool LaunchIsolatedClientInstance() {
        if (m_selectedClientPath.empty()) {
            std::cout << "[!] Please select game client executable path first!" << std::endl;
            return false;
        }

        DWORD instanceSlot = static_cast<DWORD>(m_launchedClients.size() + 1);

        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = { 0 };

        // Launch game process suspended to inject memory virtualization stealthily
        BOOL created = CreateProcessW(
            m_selectedClientPath.c_str(),
            NULL, NULL, NULL, FALSE,
            CREATE_SUSPENDED,
            NULL, NULL, &si, &pi
        );

        if (!created) {
            std::cout << "[!] Failed to launch client process!" << std::endl;
            return false;
        }

        // Apply 1-to-1 Per-Client Virtual PC Hardware Identity Isolation
        GlobalEngine::VirtualPCIdentity vpc = m_vmEngine.CreateUniquePCIdentity(pi.dwProcessId, instanceSlot);

        // Apply 1-to-1 Dedicated Cloudflare WARP / Proxy IP Tunneling
        GlobalEngine::DedicatedClientRoute route = m_tunnelEngine.AssignDedicatedIpToClient(pi.dwProcessId, instanceSlot);

        // Inject Ring-3 & Ring-0 VM Isolation Hooks into suspended process
        StealthMultiInstanceCloaker::ApplyVirtualMachineIsolation(pi.hProcess, pi.dwProcessId, instanceSlot, vpc, route);

        // Resume client execution
        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        m_launchedClients.push_back({ instanceSlot, pi.dwProcessId, m_selectedClientPath, vpc, route });

        std::cout << "[+] [xbabazwenbirkan] Successfully launched isolated Client #" << instanceSlot 
                  << " (PID: " << pi.dwProcessId << ") with Dedicated IP & Unique System Identity!" << std::endl;

        return true;
    }

    size_t GetTotalActiveClients() const { return m_launchedClients.size(); }
};

} // namespace xbabazwenbirkan
