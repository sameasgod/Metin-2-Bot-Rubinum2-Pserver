#pragma once
#include "IMemoryEngine.hpp"
#include <iostream>

namespace GlobalEngine {

#pragma pack(push, 1)
struct VisualItemData {
    uint32_t itemVnum;
    uint8_t  count;
    uint8_t  flags;
    uint8_t  antiFlags;
    uint32_t sockets[6];
    int32_t  attributes[7][2]; // Type and Value
};
#pragma pack(pop)

class VisualItemSpoofer {
private:
    IMemoryEngine* m_engine;

public:
    explicit VisualItemSpoofer(IMemoryEngine* engine) : m_engine(engine) {}

    // Visual Item Plus Level Spoofing (Client-Side Memory Illusion)
    bool SpoofItemVisualPlus(uintptr_t clientItemInstanceAddress, uint32_t newPlusVnum) {
        if (!m_engine || !clientItemInstanceAddress) return false;

        std::cout << "[VisualSpoofer] Overriding Client-Side Item Instance VNUM to: " << newPlusVnum << std::endl;
        
        // Write local visual VNUM to CPythonItem / CPythonPlayer inventory cache
        bool status = m_engine->WriteMemory(clientItemInstanceAddress, &newPlusVnum, sizeof(uint32_t));
        
        if (status) {
            std::cout << "[+] Client Visual Illusion Active: Item rendered locally as +9!" << std::endl;
        }
        return status;
    }

    // Visual Refine Dialog Blessing Scroll Illusion
    bool SpoofRefineDialogScrollRequirement(uintptr_t refineDialogAddress) {
        if (!m_engine || !refineDialogAddress) return false;

        uint8_t scrollSatisfiedValue = 1; // Mark requirement visually true
        std::cout << "[VisualSpoofer] Overriding Client Refine Dialog Blessing Scroll Visual Check..." << std::endl;
        
        // Overrides local visual check flag in PyRefineDialog
        return m_engine->WriteMemory(refineDialogAddress + 0x18, &scrollSatisfiedValue, sizeof(uint8_t));
    }
};

} // namespace GlobalEngine
