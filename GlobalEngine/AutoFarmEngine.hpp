#pragma once
#include "IMemoryEngine.hpp"
#include "ServerProfile.hpp"
#include "HumanizedInput.hpp"
#include <iostream>
#include <vector>
#include <chrono>

namespace GlobalEngine {

enum EFarmMode {
    FARM_MODE_METIN_STONES = 0, // Metin Stone Farmer (Skill Books, Soul Stones, Yang)
    FARM_MODE_BOSS_HUNTER  = 1, // Boss & Champion Mob Farmer
    FARM_MODE_MINING       = 2, // Ore & Vein Mining (Diamond, Ebony, Pearl Ore)
    FARM_MODE_FISHING      = 3, // Auto-Fishing (Clams, Pearls)
    FARM_MODE_DUNGEON      = 4  // Devil's Tower / Dungeon Loop
};

class AutoFarmEngine {
private:
    IMemoryEngine* m_engine;
    ServerProfile m_profile;
    HumanizedInput m_input;
    EFarmMode m_currentMode = FARM_MODE_METIN_STONES;

    bool m_isFarmRunning = false;
    uint32_t m_totalYangCollected = 0;
    uint32_t m_totalMetinsDestroyed = 0;
    uint32_t m_totalOresMined = 0;

public:
    AutoFarmEngine(IMemoryEngine* engine, const ServerProfile& profile)
        : m_engine(engine), m_profile(profile), m_input(engine) {}

    void SetFarmMode(EFarmMode mode) {
        m_currentMode = mode;
        std::cout << "[FarmEngine] Farm Mode Changed to: " << (int)mode << std::endl;
    }

    void StartFarming() {
        m_isFarmRunning = true;
        std::cout << "[+] Global Multi-Stream Farming Engine Started on " << m_profile.serverName << std::endl;
    }

    void StopFarming() {
        m_isFarmRunning = false;
        std::cout << "[*] Farming Engine Stopped." << std::endl;
    }

    // Execute Main Farming Cycle based on selected Mode
    void ExecuteFarmCycle() {
        if (!m_isFarmRunning || !m_engine) return;

        switch (m_currentMode) {
        case FARM_MODE_METIN_STONES:
            // Scan for Metin Stones (BK / Soul Stone / Yang Farm)
            m_input.HumanSleep(100, 20);
            break;
        case FARM_MODE_BOSS_HUNTER:
            // Scan for Boss Entities
            m_input.HumanSleep(120, 25);
            break;
        case FARM_MODE_MINING:
            // Target Mining Veins & Mine Ores
            m_input.HumanSleep(200, 50);
            break;
        case FARM_MODE_FISHING:
            // Auto Fishing Bait & Cast Loop
            m_input.HumanSleep(300, 50);
            break;
        case FARM_MODE_DUNGEON:
            // Dungeon Auto-Run
            m_input.HumanSleep(150, 30);
            break;
        }
    }

    // Get Analytics
    uint32_t GetTotalMetins() const { return m_totalMetinsDestroyed; }
    uint32_t GetTotalYang() const { return m_totalYangCollected; }
};

} // namespace GlobalEngine
