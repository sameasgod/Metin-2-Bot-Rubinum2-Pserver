#pragma once
#include "IMemoryEngine.hpp"
#include "../KernelDriver/KernelDriver.h"
#include <tlhelp32.h>
#include <iostream>

namespace GlobalEngine {

class KernelMemoryEngine : public IMemoryEngine {
private:
    HANDLE m_driverHandle = INVALID_HANDLE_VALUE;
    DWORD m_targetProcessId = 0;
    bool m_isKernelActive = false;

public:
    KernelMemoryEngine() {
        m_driverHandle = CreateFileW(
            L"\\\\.\\Ring0StealthEngine",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        if (m_driverHandle != INVALID_HANDLE_VALUE) {
            m_isKernelActive = true;
            std::cout << "[+] Kernel Driver Connection Established (Ring 0 Active)" << std::endl;
        } else {
            std::cout << "[!] Warning: Kernel Driver not loaded. Operating in Ring 3 fallback mode." << std::endl;
        }
    }

    ~KernelMemoryEngine() override {
        if (m_driverHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_driverHandle);
        }
    }

    bool Initialize(DWORD processId) override {
        m_targetProcessId = processId;
        return m_targetProcessId != 0;
    }

    uintptr_t GetModuleBaseAddress(const std::wstring& moduleName) override {
        if (m_targetProcessId == 0) return 0;

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_targetProcessId);
        if (hSnap == INVALID_HANDLE_VALUE) return 0;

        MODULEENTRY32W modEntry;
        modEntry.dwSize = sizeof(modEntry);

        uintptr_t modBase = 0;
        if (Module32FirstW(hSnap, &modEntry)) {
            do {
                if (_wcsicmp(modEntry.szModule, moduleName.c_str()) == 0) {
                    modBase = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32NextW(hSnap, &modEntry));
        }
        CloseHandle(hSnap);
        return modBase;
    }

    bool ReadMemory(uintptr_t address, void* buffer, size_t size) override {
        if (!buffer || size == 0 || m_targetProcessId == 0) return false;

        if (m_isKernelActive) {
            RING0_MEMORY_REQUEST req{};
            req.ProcessId = m_targetProcessId;
            req.SourceAddress = (ULONGLONG)address;
            req.TargetAddress = (ULONGLONG)buffer;
            req.Size = (ULONGLONG)size;

            DWORD bytesReturned = 0;
            return DeviceIoControl(m_driverHandle, IOCTL_RING0_READ_MEMORY, &req, sizeof(req), &req, sizeof(req), &bytesReturned, NULL) != FALSE;
        } else {
            // Ring 3 Fallback Read
            HANDLE hProc = OpenProcess(PROCESS_VM_READ, FALSE, m_targetProcessId);
            if (!hProc) return false;
            SIZE_TYPE bytesRead = 0;
            BOOL ok = ReadProcessMemory(hProc, (LPCVOID)address, buffer, size, &bytesRead);
            CloseHandle(hProc);
            return ok != FALSE;
        }
    }

    bool WriteMemory(uintptr_t address, const void* buffer, size_t size) override {
        if (!buffer || size == 0 || m_targetProcessId == 0) return false;

        if (m_isKernelActive) {
            RING0_MEMORY_REQUEST req{};
            req.ProcessId = m_targetProcessId;
            req.SourceAddress = (ULONGLONG)buffer;
            req.TargetAddress = (ULONGLONG)address;
            req.Size = (ULONGLONG)size;

            DWORD bytesReturned = 0;
            return DeviceIoControl(m_driverHandle, IOCTL_RING0_WRITE_MEMORY, &req, sizeof(req), &req, sizeof(req), &bytesReturned, NULL) != FALSE;
        } else {
            // Ring 3 Fallback Write
            HANDLE hProc = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, m_targetProcessId);
            if (!hProc) return false;
            SIZE_TYPE bytesWritten = 0;
            BOOL ok = WriteProcessMemory(hProc, (LPVOID)address, buffer, size, &bytesWritten);
            CloseHandle(hProc);
            return ok != FALSE;
        }
    }

    bool HideProcessFromKernel(DWORD processId) override {
        if (!m_isKernelActive) return false;
        RING0_HIDE_PROCESS_REQUEST req{};
        req.ProcessId = processId;
        DWORD bytesReturned = 0;
        return DeviceIoControl(m_driverHandle, IOCTL_RING0_HIDE_PROCESS, &req, sizeof(req), &req, sizeof(req), &bytesReturned, NULL) != FALSE;
    }

    bool InjectStealthInput(uint32_t inputType, uint16_t buttonFlags, int16_t dx, int16_t dy, uint16_t makeCode) override {
        if (!m_isKernelActive) return false;
        RING0_INPUT_REQUEST req{};
        req.InputType = inputType;
        req.ButtonFlags = buttonFlags;
        req.LastX = dx;
        req.LastY = dy;
        req.MakeCode = makeCode;
        DWORD bytesReturned = 0;
        return DeviceIoControl(m_driverHandle, IOCTL_RING0_INJECT_INPUT, &req, sizeof(req), &req, sizeof(req), &bytesReturned, NULL) != FALSE;
    }

    bool IsKernelModeActive() const override {
        return m_isKernelActive;
    }
};

} // namespace GlobalEngine
