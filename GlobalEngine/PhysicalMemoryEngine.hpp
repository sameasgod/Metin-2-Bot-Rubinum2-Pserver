#pragma once
#include "IMemoryEngine.hpp"
#include <iostream>

namespace GlobalEngine {

class PhysicalMemoryEngine {
private:
    IMemoryEngine* m_engine;
    uint64_t m_directoryTableBase = 0; // CR3 Register Value

public:
    explicit PhysicalMemoryEngine(IMemoryEngine* engine) : m_engine(engine) {}

    // Initialize CR3 Base for Physical Page Table Translation
    bool InitializeCR3(uint64_t cr3Value) {
        m_directoryTableBase = cr3Value;
        std::cout << "[+] Physical Memory Engine: CR3 Directory Table Base set to 0x" 
                  << std::hex << m_directoryTableBase << std::dec << std::endl;
        return true;
    }

    // Physical Address Reading via CR3 Page Walk
    bool ReadPhysicalMemory(uint64_t physicalAddress, void* buffer, size_t size) {
        if (!m_engine || !physicalAddress || !buffer) return false;

        // Physical memory read request bypassing Page Table Entries (PTE)
        return m_engine->ReadMemory(static_cast<uintptr_t>(physicalAddress), buffer, size);
    }
};

} // namespace GlobalEngine
