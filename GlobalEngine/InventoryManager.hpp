#pragma once
#include "IMemoryEngine.hpp"
#include <iostream>
#include <vector>

namespace GlobalEngine {

struct ItemFilterConfig {
    bool keepSkillBooks = true;       // Beceri Kitabı (BK)
    bool keepSoulStones = true;       // Ruh Taşı
    bool keepBlessingScrolls = true;  // Kutsama Kağıdı
    bool keepPearlsAndClams = true;   // İnci ve İstiridye
    bool keepOresAndGems = true;      // Cevherler (Abanoz, Elmas vb.)
    bool autoSellTrashItems = true;   // Çöp İtemleri Satıcıya Otomatik Sat
};

class InventoryManager {
private:
    IMemoryEngine* m_engine;
    ItemFilterConfig m_filter;

public:
    explicit InventoryManager(IMemoryEngine* engine) : m_engine(engine) {}

    void SetFilterConfig(const ItemFilterConfig& filter) {
        m_filter = filter;
        std::cout << "[InventoryManager] Filter Rules Updated!" << std::endl;
    }

    // Auto-Loot Filtering Decision
    bool ShouldLootItem(uint32_t itemVnum) {
        // BK Vnum Range Check (50000 - 50500)
        if (itemVnum >= 50000 && itemVnum <= 50500) return m_filter.keepSkillBooks;
        // Ruh Taşı Vnum Check (50051)
        if (itemVnum == 50051) return m_filter.keepSoulStones;
        // Kutsama Kağıdı Vnum Check (27987, 27999)
        if (itemVnum == 27987 || itemVnum == 27999) return m_filter.keepBlessingScrolls;
        
        return true; // Default loot
    }

    // Auto Sell Trash Items to General Store NPC
    void SellTrashItemsToNPC(uintptr_t npcAddress) {
        if (!m_filter.autoSellTrashItems || !m_engine || !npcAddress) return;

        std::cout << "[+] Auto-Selling Trash Equipment to General Store NPC..." << std::endl;
        // Memory packet or input sequence for opening NPC shop and selling slots 0..44 trash
    }
};

} // namespace GlobalEngine
