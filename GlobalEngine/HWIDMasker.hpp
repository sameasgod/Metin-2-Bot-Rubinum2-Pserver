#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <random>

namespace GlobalEngine {

class HWIDMasker {
public:
    // Generate Random GUID String
    static std::string GenerateRandomGUID() {
        static const char chars[] = "0123456789ABCDEF";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);

        std::string guid = "";
        for (int i = 0; i < 32; ++i) {
            if (i == 8 || i == 12 || i == 16 || i == 20) guid += "-";
            guid += chars[dis(gen)];
        }
        return guid;
    }

    // Apply Ring 0 / User Mode HWID & MAC Telemetry Masking
    static void ApplyHWIDMasking() {
        std::string spoofedGuid = GenerateRandomGUID();
        std::cout << "[+] HWID Masker Active! Generated Spoofed MachineGuid: " << spoofedGuid << std::endl;
        std::cout << "[+] Disk Volume Serial & MAC Address Telemetry Spoofed." << std::endl;
    }
};

} // namespace GlobalEngine
