#pragma once
#include "IMemoryEngine.hpp"
#include <windows.h>
#include <iostream>
#include <string>

namespace GlobalEngine {

typedef void(*tPyRun_SimpleString)(const char* command);

class PythonScriptInjector {
private:
    IMemoryEngine* m_engine;
    uintptr_t m_pyRunSimpleStringAddr = 0;

public:
    explicit PythonScriptInjector(IMemoryEngine* engine) : m_engine(engine) {
        HMODULE hPy = GetModuleHandleA("python27.dll");
        if (!hPy) hPy = GetModuleHandleA("python22.dll");
        if (hPy) {
            m_pyRunSimpleStringAddr = (uintptr_t)GetProcAddress(hPy, "PyRun_SimpleString");
            if (m_pyRunSimpleStringAddr) {
                std::cout << "[+] PyRun_SimpleString Export Found: 0x" << std::hex << m_pyRunSimpleStringAddr << std::dec << std::endl;
            }
        }
    }

    // Execute Python Command string directly in Metin2 CPython runtime
    bool RunPythonCommand(const std::string& pyScript) {
        if (!m_pyRunSimpleStringAddr) return false;

        std::cout << "[PythonScriptInjector] Executing script in CPython runtime: " << pyScript << std::endl;
        tPyRun_SimpleString PyRun_SimpleString_fn = (tPyRun_SimpleString)m_pyRunSimpleStringAddr;
        PyRun_SimpleString_fn(pyScript.c_str());
        return true;
    }

    // High-Level Python Metin2 Automation Snippets
    void ExecuteAutoPick() {
        RunPythonCommand("import player\nplayer.PickCloseItem()");
    }

    void ExecuteAutoAttack(uint32_t vid) {
        std::string script = "import player, chr\nplayer.SetAttackTarget(" + std::to_string(vid) + ")\nchr.SelectInstance(" + std::to_string(vid) + ")";
        RunPythonCommand(script);
    }
};

} // namespace GlobalEngine
