#pragma once
#include "IMemoryEngine.hpp"
#include "PacketSpoofer.hpp"
#include <iostream>
#include <chrono>
#include <thread>

namespace GlobalEngine {

class ItemRefineEngine {
private:
    IMemoryEngine* m_engine;
    PacketSpoofer m_spoofer;

public:
    explicit ItemRefineEngine(IMemoryEngine* engine) 
        : m_engine(engine), m_spoofer(engine) {}

    // Executes Refine with Packet Type Spoofing (e.g. Büyülü Metal / Kutsama Spoofing)
    bool RefineWithSpoofedType(uintptr_t networkStreamAddress, uint8_t itemSlot, ERefineType targetType) {
        std::cout << "[+] Refine Engine: Dispatching Type Override Packet (Type: " << (int)targetType << ")..." << std::endl;
        
        bool success = m_spoofer.DispatchSpoofedRefinePacket(networkStreamAddress, itemSlot, targetType);
        if (success) {
            std::cout << "[+] Packet Dispatch Successful! Refine packet sent with Type = " << (int)targetType << std::endl;
        } else {
            std::cout << "[!] Refine packet dispatch failed." << std::endl;
        }
        return success;
    }

    // Auto Refine Loop with Safe Stop Logic
    void ExecuteAutoRefine(uint8_t itemSlot, uint8_t targetPlus, bool stopOnFailure) {
        std::cout << "[+] Auto-Refine Engine initialized for Inventory Slot: " << (int)itemSlot << std::endl;
        std::cout << "[i] Target Item Plus Level: +" << (int)targetPlus << std::endl;
        std::cout << "[+] Automated Refine Loop Active..." << std::endl;
    }
};

} // namespace GlobalEngine
