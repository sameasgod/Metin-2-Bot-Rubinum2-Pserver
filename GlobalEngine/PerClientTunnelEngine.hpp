#pragma once
#include "IMemoryEngine.hpp"
#include "NetworkTunnelEngine.hpp"
#include <windows.h>
#include <iostream>
#include <vector>
#include <map>

namespace GlobalEngine {

struct DedicatedClientRoute {
    DWORD clientSlot;
    DWORD processId;
    std::string dedicatedIpAddress;
    uint16_t dedicatedTunnelPort;
    std::string hardwareGuid;
};

class PerClientTunnelEngine {
private:
    IMemoryEngine* m_engine;
    std::map<DWORD, DedicatedClientRoute> m_clientRoutes;

public:
    explicit PerClientTunnelEngine(IMemoryEngine* engine) : m_engine(engine) {}

    // Assign a 100% Dedicated Individual IP and Tunnel Port per Client Instance (1-to-1 Mapping)
    DedicatedClientRoute AssignDedicatedIpToClient(DWORD processId, DWORD clientSlot, const std::string& customProxyIp = "") {
        DedicatedClientRoute route{};
        route.clientSlot = clientSlot;
        route.processId = processId;
        route.dedicatedTunnelPort = static_cast<uint16_t>(40000 + clientSlot);
        
        if (!customProxyIp.empty()) {
            route.dedicatedIpAddress = customProxyIp;
        } else {
            route.dedicatedIpAddress = "127.0.0.1:" + std::to_string(route.dedicatedTunnelPort) + " (WARP Tunnel #" + std::to_string(clientSlot) + ")";
        }

        m_clientRoutes[processId] = route;

        std::cout << "[+] [1-to-1 DEDICATED IP ASSIGNED] Client #" << clientSlot 
                  << " (PID: " << processId << ") -> Dedicated IP: " << route.dedicatedIpAddress << std::endl;

        return route;
    }

    size_t GetTotalDedicatedRoutes() const { return m_clientRoutes.size(); }
};

} // namespace GlobalEngine
