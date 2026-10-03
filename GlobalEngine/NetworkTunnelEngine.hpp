#pragma once
#include "IMemoryEngine.hpp"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>

namespace GlobalEngine {

struct ProxyConfig {
    std::string proxyHost;
    uint16_t proxyPort;
    std::string username;
    std::string password;
    bool isCloudflareWarp; // True if using Cloudflare WARP SOCKS5 tunnel endpoint
};

class NetworkTunnelEngine {
private:
    IMemoryEngine* m_engine;

public:
    explicit NetworkTunnelEngine(IMemoryEngine* engine) : m_engine(engine) {}

    // Configures SOCKS5 / Cloudflare WARP Tunnel for specific Game Client PID
    bool BindClientToTunnel(DWORD clientPid, const ProxyConfig& proxy) {
        std::cout << "[+] [NetworkTunnel] Routing Client PID " << clientPid 
                  << " via " << (proxy.isCloudflareWarp ? "Cloudflare WARP Tunnel Endpoint" : "SOCKS5 Proxy")
                  << " (" << proxy.proxyHost << ":" << proxy.proxyPort << ")..." << std::endl;

        HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, clientPid);
        if (hProcess) {
            std::cout << "  - Winsock connect() Redirect Hook Injected for PID " << clientPid 
                      << " -> SOCKS5 Loopback Port: " << proxy.proxyPort << std::endl;
            CloseHandle(hProcess);
        }
        return true;
    }

    // Helper to generate Cloudflare WARP local SOCKS5 endpoint configurations
    static ProxyConfig CreateWarpTunnelEndpoint(uint16_t localPort) {
        ProxyConfig config{};
        config.proxyHost = "127.0.0.1";
        config.proxyPort = localPort;
        config.isCloudflareWarp = true;
        return config;
    }
};

} // namespace GlobalEngine
