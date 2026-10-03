#include "GlobalBotController.h"
#include <iostream>
#include <tlhelp32.h>
#include <thread>

namespace GlobalBot {

static DWORD FindProcessIdByName(const std::wstring& processName) {
    DWORD pid = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(hSnap, &pe)) {
            do {
                if (_wcsicmp(pe.szExeFile, processName.c_str()) == 0) {
                    pid = pe.th32ProcessID;
                    break;
                }
            } while (Process32NextW(hSnap, &pe));
        }
        CloseHandle(hSnap);
    }
    return pid;
}

Controller::Controller() {
    m_memoryEngine = std::make_unique<GlobalEngine::KernelMemoryEngine>();
    m_inputController = std::make_unique<GlobalEngine::HumanizedInput>(m_memoryEngine.get());
}

Controller::~Controller() {
    StopGlobalEngine();
}

bool Controller::StartGlobalEngine(const std::string& profileJsonPath) {
    GlobalEngine::AntiCheatBypass::RandomizeConsoleTitle();
    GlobalEngine::AntiCheatBypass::ApplyHWIDSpoofing();

    m_profile.PrintProfileInfo();

    std::wstring exeWStr(m_profile.targetExecutable.begin(), m_profile.targetExecutable.end());
    m_targetProcessId = FindProcessIdByName(exeWStr);

    if (m_targetProcessId == 0) {
        std::cout << "[!] Could not find running target process: " << m_profile.targetExecutable << std::endl;
        return false;
    }

    std::cout << "[+] Found Target Process PID: " << m_targetProcessId << std::endl;

    if (!m_memoryEngine->Initialize(m_targetProcessId)) {
        std::cout << "[!] Failed to initialize Memory Engine." << std::endl;
        return false;
    }

    m_baseAddress = m_memoryEngine->GetModuleBaseAddress(exeWStr);
    std::cout << "[+] Target Base Address: 0x" << std::hex << m_baseAddress << std::dec << std::endl;

    // Apply Ring 0 DKOM process cloaking
    if (m_memoryEngine->IsKernelModeActive()) {
        GlobalEngine::AntiCheatBypass::EnableKernelProcessStealth(m_memoryEngine.get(), m_targetProcessId);
    }

    m_isRunning = true;
    return true;
}

void Controller::StopGlobalEngine() {
    m_isRunning = false;
}

void Controller::ToggleWallhack() {
    m_wallhackState = !m_wallhackState;
    std::cout << "[i] Wallhack toggled: " << (m_wallhackState ? "ENABLED" : "DISABLED") << std::endl;
}

void Controller::ToggleFreezeOnPlayer() {
    m_freezeOnPlayer = !m_freezeOnPlayer;
    std::cout << "[i] Freeze on nearby player toggled: " << (m_freezeOnPlayer ? "ENABLED" : "DISABLED") << std::endl;
}

bool Controller::IsKernelMode() const {
    return m_memoryEngine && m_memoryEngine->IsKernelModeActive();
}

void Controller::RunBotLoop() {
    std::cout << "[+] Global Out-Of-Process Stealth Loop Started..." << std::endl;

    while (m_isRunning) {
        if (m_memoryEngine) {
            // High-precision out-of-process state evaluation using Ring 0 memory reads
            m_inputController->HumanSleep(50, 10);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

} // namespace GlobalBot
