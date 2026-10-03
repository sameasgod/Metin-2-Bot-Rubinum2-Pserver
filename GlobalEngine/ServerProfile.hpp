#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

namespace GlobalEngine {

struct EntityOffsets {
    size_t nameOffset = 0x0010;
    size_t positionOffset = 0x02DC;
    size_t isPlayerOffset = 0x02E8;
    size_t isRenderedOffset = 0x0340;
    size_t attackStanceOffset = 0x06E0;
    size_t mobIdOffset = 0x06E8;
    size_t mobDiedOffset = 0x0730;
    size_t uidOffset = 0x1C54;
    size_t skipCollisionOffset = 0x1C5C;
};

struct SignaturePattern {
    std::string name;
    std::string pattern;
    std::string mask;
    size_t offset = 0;
};

class ServerProfile {
public:
    std::string serverName;
    std::string targetExecutable;
    
    int metinIdStart = 8001;
    int metinIdEnd = 8416;
    int oreIdStart = 30301;
    int oreIdEnd = 30306;

    int preferredTargetDistance = 2000;
    bool enableWallhack = true;
    bool enableAutoLoot = true;
    bool freezeOnPlayerNearby = true;
    bool enableWhisperProtection = true;

    EntityOffsets offsets;
    std::vector<SignaturePattern> patterns;

    ServerProfile() {
        // Default Metin2 Profile Configuration (Rubinum / Standard PServer)
        serverName = "Rubinum / Standard Metin2 PServer";
        targetExecutable = "rbclient.exe";

        // Pickup pattern
        patterns.push_back({
            "pickup",
            "\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x53\x56\x57\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x45\x00\x64\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF0\x8D\x00\x00\x8B\x01",
            "xxxx?x????x?????????xxxx????????x?x?????????xx??xx",
            0
        });

        // Whisper pattern
        patterns.push_back({
            "recvWhisper",
            "\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x81\xEC\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xF0\x53\x56\x57\x50\x8D\x45\xF4\x64\xA3\x00\x00\x00\x00\x8B\xF9\x8D\x85\x00\x00\x00\x00",
            "xxxxxx????xx????xxx????x????xxxxxxxxxxxxxx????xxxx????",
            0
        });

        // Character Manager pattern
        patterns.push_back({
            "recvDead",
            "\x55\x8B\xEC\x83\xEC\x10\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xFC\x53\x8D\x45\xF0",
            "xxxxxxx????xxxxxxxxx",
            0
        });
    }

    bool IsMetinStone(int mobId) const {
        return mobId >= metinIdStart && mobId <= metinIdEnd;
    }

    bool IsOreNode(int mobId) const {
        return mobId >= oreIdStart && mobId <= oreIdEnd;
    }

    void PrintProfileInfo() const {
        std::cout << "[Profile] Server Profile: " << serverName << std::endl;
        std::cout << "[Profile] Executable: " << targetExecutable << std::endl;
        std::cout << "[Profile] Metin ID Range: " << metinIdStart << " - " << metinIdEnd << std::endl;
        std::cout << "[Profile] Ore ID Range: " << oreIdStart << " - " << oreIdEnd << std::endl;
    }
};

} // namespace GlobalEngine
