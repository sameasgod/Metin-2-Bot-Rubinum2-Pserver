#pragma once
#include "NetworkTunnelEngine.hpp"
#include "HWIDMasker.hpp"
#include <vector>
#include <iostream>

namespace GlobalEngine {

struct ClientCluster {
    size_t clusterId;
    std::vector<DWORD> clientPids;
    std::string spoofedHwid;
    ProxyConfig tunnelConfig;
};

class ClientLimitBypassEngine {
private:
    IMemoryEngine* m_engine;
    NetworkTunnelEngine m_tunnelEngine;
    std::vector<ClientCluster> m_clusters;
    const size_t MAX_CLIENTS_PER_CLUSTER = 3;

public:
    explicit ClientLimitBypassEngine(IMemoryEngine* engine)
        : m_engine(engine), m_tunnelEngine(engine) {}

    // Register & Assign Client PID to a 3-Client Cluster Group
    void AssignClientToBypassCluster(DWORD clientPid) {
        if (m_clusters.empty() || m_clusters.back().clientPids.size() >= MAX_CLIENTS_PER_CLUSTER) {
            // Create New Cluster Group for the next batch of 3 Clients
            size_t newClusterId = m_clusters.size() + 1;
            uint16_t warpPort = static_cast<uint16_t>(40000 + newClusterId);
            
            ClientCluster newCluster{};
            newCluster.clusterId = newClusterId;
            newCluster.spoofedHwid = HWIDMasker::GenerateRandomGUID();
            newCluster.tunnelConfig = NetworkTunnelEngine::CreateWarpTunnelEndpoint(warpPort);

            m_clusters.push_back(newCluster);
            std::cout << "[+] Created New 3-Client Cluster Group #" << newClusterId 
                      << " (Spoofed HWID: " << newCluster.spoofedHwid 
                      << " | Cloudflare WARP Port: " << warpPort << ")" << std::endl;
        }

        ClientCluster& currentCluster = m_clusters.back();
        currentCluster.clientPids.push_back(clientPid);

        std::cout << "[BypassEngine] Bound Client PID " << clientPid 
                  << " to Cluster #" << currentCluster.clusterId 
                  << " (" << currentCluster.clientPids.size() << "/" << MAX_CLIENTS_PER_CLUSTER << " Slots Used)" << std::endl;

        m_tunnelEngine.BindClientToTunnel(clientPid, currentCluster.tunnelConfig);
    }

    size_t GetTotalClusters() const { return m_clusters.size(); }
};

} // namespace GlobalEngine
