#pragma once
#include "IMemoryEngine.hpp"
#include <vector>
#include <iostream>

namespace GlobalEngine {

class PatternScanner {
public:
    static uintptr_t FindPattern(IMemoryEngine* engine, uintptr_t baseAddress, size_t imageSize, const char* pattern, const char* mask) {
        if (!engine || !baseAddress || imageSize == 0 || !pattern || !mask) return 0;

        size_t patternLen = strlen(mask);
        const size_t chunkSize = 0x4000; // 16 KB chunks for high performance scanning
        std::vector<uint8_t> buffer(chunkSize);

        for (size_t offset = 0; offset < imageSize; offset += chunkSize - patternLen) {
            size_t readSize = (chunkSize < (imageSize - offset)) ? chunkSize : (imageSize - offset);

            if (!engine->ReadMemory(baseAddress + offset, buffer.data(), readSize)) {
                continue;
            }

            for (size_t i = 0; i < readSize - patternLen; ++i) {
                bool found = true;
                for (size_t j = 0; j < patternLen; ++j) {
                    if (mask[j] != '?' && static_cast<uint8_t>(pattern[j]) != buffer[i + j]) {
                        found = false;
                        break;
                    }
                }
                if (found) {
                    return baseAddress + offset + i;
                }
            }
        }
        return 0;
    }
};

} // namespace GlobalEngine
