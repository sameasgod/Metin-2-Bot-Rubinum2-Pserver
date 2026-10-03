#include "SteinBoxer/GlobalBotController.h"
#include <iostream>
#include <conio.h>

int main(int argc, char* argv[]) {
    std::cout << "==========================================================" << std::endl;
    std::cout << "   Metin2 Global Ring-0 Full Anonymity Engine v2.0       " << std::endl;
    std::cout << "   - Multi-Server Profile Loader (Rubinum / Svens / Uriel)" << std::endl;
    std::cout << "   - Ring 0 Kernel Memory Access (MmCopyVirtualMemory)   " << std::endl;
    std::cout << "   - Kernel DKOM Stealth & Process Cloaking             " << std::endl;
    std::cout << "   - HWID & Telemetry Anonymization                      " << std::endl;
    std::cout << "   - Humanized AI Input Simulation (Bézier Trajectories) " << std::endl;
    std::cout << "==========================================================" << std::endl << std::endl;

    GlobalBot::Controller botController;

    std::string profilePath = "profiles/rubinum_profile.json";
    if (argc > 1) {
        profilePath = argv[1];
    }

    std::cout << "[*] Initializing Global Engine with profile: " << profilePath << std::endl;

    if (!botController.StartGlobalEngine(profilePath)) {
        std::cout << "[!] Failed to launch Global Bot Engine. Press any key to exit..." << std::endl;
        _getch();
        return 1;
    }

    std::cout << std::endl;
    std::cout << "[Controls]" << std::endl;
    std::cout << "  F1  : Toggle Bot Engine" << std::endl;
    std::cout << "  F2  : Toggle Player Proximity Safety Freeze" << std::endl;
    std::cout << "  F4  : Toggle Stealth Wallhack" << std::endl;
    std::cout << "  END : Shutdown & Clean Traces" << std::endl << std::endl;

    std::thread botThread([&botController]() {
        botController.RunBotLoop();
    });

    while (true) {
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            std::cout << "[*] Shutting down Global Bot Engine..." << std::endl;
            botController.StopGlobalEngine();
            break;
        }

        if (GetAsyncKeyState(VK_F4) & 0x8000) {
            botController.ToggleWallhack();
            Sleep(300);
        }

        if (GetAsyncKeyState(VK_F2) & 0x8000) {
            botController.ToggleFreezeOnPlayer();
            Sleep(300);
        }

        Sleep(50);
    }

    if (botThread.joinable()) {
        botThread.join();
    }

    std::cout << "[+] Engine stopped cleanly. Goodbye!" << std::endl;
    return 0;
}
