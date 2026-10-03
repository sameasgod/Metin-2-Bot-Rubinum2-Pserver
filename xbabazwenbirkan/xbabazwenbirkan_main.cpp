#include "xbabazwenbirkan_launcher.hpp"
#include <iostream>
#include <conio.h>

int main(int argc, char* argv[]) {
    SetConsoleTitleA("xbabazwenbirkan - Per-Client Isolated Multi-Launcher v1.0");

    std::cout << "==========================================================" << std::endl;
    std::cout << "        xbabazwenbirkan - Dedicated Client Spoofer      " << std::endl;
    std::cout << "   - 1-to-1 Per-Client Dedicated IP & WARP Tunneling    " << std::endl;
    std::cout << "   - Individual Virtual PC Identity (SMBIOS, MAC, GUID)  " << std::endl;
    std::cout << "   - Memory-Stealth Injection (Zero VM Overhead)         " << std::endl;
    std::cout << "==========================================================" << std::endl << std::endl;

    xbabazwenbirkan::LauncherEngine launcher;

    std::wstring clientExePath = L"metin2client.exe";
    if (argc > 1) {
        std::string pathArg = argv[1];
        clientExePath = std::wstring(pathArg.begin(), pathArg.end());
    }

    launcher.SetClientExecutablePath(clientExePath);

    std::cout << "[Controls]" << std::endl;
    std::cout << "  F1  : Launch New Isolated Client Instance (Dedicated IP + Unique PC)" << std::endl;
    std::cout << "  END : Exit Launcher" << std::endl << std::endl;

    while (true) {
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            break;
        }

        if (GetAsyncKeyState(VK_F1) & 0x8000) {
            launcher.LaunchIsolatedClientInstance();
            Sleep(500);
        }

        Sleep(50);
    }

    std::cout << "[+] xbabazwenbirkan Launcher stopped cleanly. Goodbye!" << std::endl;
    return 0;
}
