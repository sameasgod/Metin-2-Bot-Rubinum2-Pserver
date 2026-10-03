#pragma once
#include "../GlobalEngine/KernelMemoryEngine.hpp"
#include "../GlobalEngine/ServerProfile.hpp"
#include "../GlobalEngine/AntiCheatBypass.hpp"
#include "../GlobalEngine/HumanizedInput.hpp"
#include <memory>
#include <atomic>

namespace GlobalBot {

class Controller {
private:
    std::unique_ptr<GlobalEngine::IMemoryEngine> m_memoryEngine;
    GlobalEngine::ServerProfile m_profile;
    std::unique_ptr<GlobalEngine::HumanizedInput> m_inputController;

    std::atomic<bool> m_isRunning{ false };
    std::atomic<bool> m_wallhackState{ false };
    std::atomic<bool> m_freezeOnPlayer{ true };

    DWORD m_targetProcessId = 0;
    uintptr_t m_baseAddress = 0;

public:
    Controller();
    ~Controller();

    bool StartGlobalEngine(const std::string& profileJsonPath);
    void StopGlobalEngine();

    void RunBotLoop();
    void ToggleWallhack();
    void ToggleFreezeOnPlayer();

    bool IsKernelMode() const;
};

} // namespace GlobalBot
