#pragma once
#include "IMemoryEngine.hpp"
#include "PacketSpoofer.hpp"
#include <iostream>

namespace GlobalEngine {

class SvsideRefineHelper {
private:
    IMemoryEngine* m_engine;
    PacketSpoofer m_spoofer;

public:
    explicit SvsideRefineHelper(IMemoryEngine* engine)
        : m_engine(engine), m_spoofer(engine) {}

    // Rapid Svside Refine Loop (Fast Packet Upgrade for Profit)
    bool FastRefineItem(uintptr_t networkStreamAddr, uint8_t slot, uint8_t targetPlus) {
        std::cout << "[SvsideRefine] Executing Fast Refine for Inventory Slot " << (int)slot << " -> Target: +" << (int)targetPlus << std::endl;
        
        // Dispatch Packet Type 2 (Büyülü Metal Override)
        return m_spoofer.DispatchSpoofedRefinePacket(networkStreamAddr, slot, REFINE_TYPE_HYEONSO);
    }
};

} // namespace GlobalEngine
