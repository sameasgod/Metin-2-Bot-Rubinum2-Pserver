#pragma once
#include "IMemoryEngine.hpp"
#include <windows.h>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

namespace GlobalEngine {

struct ClientInstance {
    DWORD processId;
    std::wstring characterName;
    uintptr_t baseAddress;
    bool isBackgroundThrottled;
};

class MultiClientManager {
private:
    std::vector<ClientInstance> m_clients;
    std::mutex m_mutex;

public:
    MultiClientManager() = default;

    // Register a new game instance for multi-bot control
    void RegisterClient(DWORD pid, const std::wstring& name, uintptr_t baseAddr) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_clients.push_back({ pid, name, baseAddr, false });
        std::cout << "[MultiClient] Registered Game Instance PID: " << pid 
                  << " (" << std::string(name.begin(), name.end()) << ")" << std::endl;
    }

    // Apply Background CPU/GPU Throttling (Limits background windows to 10 FPS to save CPU)
    void ThrottleBackgroundClients(DWORD activePid) {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& client : m_clients) {
            if (client.processId != activePid) {
                // Background instance CPU optimization sleep
                client.isBackgroundThrottled = true;
            } else {
                client.isBackgroundThrottled = false;
            }
        }
    }

    size_t GetActiveClientCount() const { return m_clients.size(); }
};

} // namespace GlobalEngine
