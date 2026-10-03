#pragma once
#include <windows.h>
#include <vector>
#include <string>

namespace GlobalEngine {

class IMemoryEngine {
public:
    virtual ~IMemoryEngine() = default;

    virtual bool Initialize(DWORD processId) = 0;
    virtual uintptr_t GetModuleBaseAddress(const std::wstring& moduleName) = 0;
    
    virtual bool ReadMemory(uintptr_t address, void* buffer, size_t size) = 0;
    virtual bool WriteMemory(uintptr_t address, const void* buffer, size_t size) = 0;

    template <typename T>
    T Read(uintptr_t address) {
        T value{};
        ReadMemory(address, &value, sizeof(T));
        return value;
    }

    template <typename T>
    bool Write(uintptr_t address, const T& value) {
        return WriteMemory(address, &value, sizeof(T));
    }

    virtual bool HideProcessFromKernel(DWORD processId) = 0;
    virtual bool InjectStealthInput(uint32_t inputType, uint16_t buttonFlags, int16_t dx, int16_t dy, uint16_t makeCode) = 0;
    virtual bool IsKernelModeActive() const = 0;
};

} // namespace GlobalEngine
