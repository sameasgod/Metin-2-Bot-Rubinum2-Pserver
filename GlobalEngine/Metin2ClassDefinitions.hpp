#pragma once
#include <windows.h>
#include <map>
#include <vector>
#include <string>

namespace GlobalEngine {

#pragma pack(push, 1)

// Metin2 Pixel Position Vector
struct TPixelPosition {
    float x, y, z;
};

// Item Position Structure
struct TItemPos {
    uint8_t window_type;
    uint16_t cell;
};

// Character Instance (CPythonInstance)
class CPythonInstance {
public:
    char pad_0000[16];
    char name[24];
    char pad_0028[692];
    TPixelPosition position;  // 0x02DC
    int32_t isPlayerCharacter; // 0x02E8
    char pad_02EC[84];
    int32_t isRendered;        // 0x0340
    char pad_0344[924];
    int32_t attackStance;      // 0x06E0
    char pad_06E4[4];
    int32_t mobID;             // 0x06E8
    char pad_06EC[68];
    int32_t mobDiedRecently;   // 0x0730
    char pad_0734[5408];
    int32_t uID;               // 0x1C54
    char pad_1C58[4];
    int8_t skipCollision;      // 0x1C5C

    TPixelPosition GetPosition() { return position; }
    int32_t GetMobID() { return mobID; }
    int32_t GetUID() { return uID; }
    bool IsDead() { return mobDiedRecently == 1; }
    void EnableCollisionBypass() { skipCollision = 1; }
    void DisableCollisionBypass() { skipCollision = 0; }
};

// Character Manager Instance Map
using TCharacterInstanceMap = std::map<DWORD, CPythonInstance*>;

// CPythonCharacterManager Class Definition (GitHub Metin2 Client Leak Standard)
class CPythonCharacterManager {
public:
    char pad_0000[0x20];
    TCharacterInstanceMap m_kAliveInstanceMap;

    static CPythonCharacterManager* Instance();
    CPythonInstance* GetMainInstance();
    CPythonInstance* GetTabNextTargetPointer(CPythonInstance* pkInstMain);
    CPythonInstance* GetInstanceByVID(DWORD dwVID);
};

// CPythonPlayer Class Definition
class CPythonPlayer {
public:
    static CPythonPlayer* Instance();
    void SetAttackTarget(DWORD dwVID);
    void PickCloseItem();
    void SendRefinePacket(BYTE bPos, BYTE bType);
    void SendClickItemPacket(DWORD dwItemID);
    TPixelPosition GetMainCharacterPosition();
};

// CPythonNetworkStream Class Definition
class CPythonNetworkStream {
public:
    static CPythonNetworkStream* Instance();
    bool SendAttackPacket(BYTE bType, DWORD dwVID);
    bool SendItemUsePacket(TItemPos pos);
    bool SendPickItemPacket(DWORD dwVID);
    bool SendRefinePacket(int iPos, int iType);
    bool SendWhisperPacket(const char* name, const char* line);
};

#pragma pack(pop)

} // namespace GlobalEngine
