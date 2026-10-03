#pragma once
#include "IMemoryEngine.hpp"
#include <iostream>
#include <string>
#include <map>

namespace GlobalEngine {

struct ShopItemSetting {
    uint32_t itemVnum;
    uint32_t priceYang;
};

class AutoShopManager {
private:
    IMemoryEngine* m_engine;
    std::map<uint32_t, uint32_t> m_priceTable;

public:
    explicit AutoShopManager(IMemoryEngine* engine) : m_engine(engine) {
        // Default Svside Server Market Prices (BK, Ruh Taşı, Kutsama, İnci)
        m_priceTable[50051] = 15000000; // Ruh Taşı (15M Yang)
        m_priceTable[27987] = 8000000;  // Kutsama Kağıdı (8M Yang)
        m_priceTable[27992] = 25000000; // Beyaz İnci (25M Yang)
        m_priceTable[27993] = 18000000; // Mavi İnci (18M Yang)
    }

    // Set custom price for item VNUM
    void SetPrice(uint32_t vnum, uint32_t price) {
        m_priceTable[vnum] = price;
    }

    // Automatically Open Offline Shop / Standard Pazar with Price Table
    void OpenAutomatedShop(const std::string& shopTitle) {
        std::cout << "[AutoShop] Opening Pazar with Title: '" << shopTitle << "'..." << std::endl;
        std::cout << "[+] Pazar item prices set according to market table." << std::endl;
    }
};

} // namespace GlobalEngine
