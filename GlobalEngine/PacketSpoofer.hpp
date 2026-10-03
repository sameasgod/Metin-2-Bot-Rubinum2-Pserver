#pragma once
#include "IMemoryEngine.hpp"
#include <iostream>

namespace GlobalEngine {

// Metin2 CG Refine Packet Header Definitions
constexpr uint8_t HEADER_CG_REFINE = 0x52; // 82

enum ERefineType : uint8_t {
    REFINE_TYPE_NORMAL      = 0, // Normal Blacksmith
    REFINE_TYPE_NOT_USED1   = 1, // Blessing Scroll (Kutsama Kağıdı)
    REFINE_TYPE_HYEONSO     = 2, // Magic Stone (Büyülü Metal)
    REFINE_TYPE_MUSIN       = 3, // Dragon Scroll (Ejderha Kağıdı)
    REFINE_TYPE_MEMORIAL    = 4  // Guild Blacksmith (Lonca Demircisi)
};

#pragma pack(push, 1)
struct TPacketCGRefine {
    uint8_t header;
    uint8_t pos;
    uint8_t type;
};
#pragma pack(pop)

class PacketSpoofer {
private:
    IMemoryEngine* m_engine;

public:
    explicit PacketSpoofer(IMemoryEngine* engine) : m_engine(engine) {}

    // Constructs a Spoofed Refine Packet Structure
    static TPacketCGRefine CreateRefinePacket(uint8_t inventorySlot, ERefineType spoofedType) {
        TPacketCGRefine packet{};
        packet.header = HEADER_CG_REFINE;
        packet.pos = inventorySlot;
        packet.type = static_cast<uint8_t>(spoofedType);
        return packet;
    }

    // Dispatches Packet Type Override (Büyülü Metal / Kutsama Spoofing)
    bool DispatchSpoofedRefinePacket(uintptr_t networkStreamAddress, uint8_t inventorySlot, ERefineType spoofedType) {
        if (!m_engine || !networkStreamAddress) return false;

        TPacketCGRefine packet = CreateRefinePacket(inventorySlot, spoofedType);
        
        std::cout << "[PacketSpoofer] Preparing Refine Packet (Header: 0x" << std::hex << (int)packet.header 
                  << ", Slot: " << std::dec << (int)packet.pos 
                  << ", Spoofed Type: " << (int)packet.type << ")" << std::endl;

        // Memory write of spoofed refine packet structure into CPythonNetworkStream send buffer
        return m_engine->WriteMemory(networkStreamAddress, &packet, sizeof(TPacketCGRefine));
    }
};

} // namespace GlobalEngine
