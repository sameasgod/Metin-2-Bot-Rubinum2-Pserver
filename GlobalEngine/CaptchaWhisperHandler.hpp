#pragma once
#include "IMemoryEngine.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <random>

namespace GlobalEngine {

class CaptchaWhisperHandler {
private:
    IMemoryEngine* m_engine;
    std::vector<std::string> m_autoResponses = {
        "sa krdsm farmdayim",
        "efendim kanka",
        "pazara bakiodum",
        "slot kesiorum birazdan donerim",
        "slm kanka"
    };

public:
    explicit CaptchaWhisperHandler(IMemoryEngine* engine) : m_engine(engine) {}

    // On Whisper Received
    void OnWhisperReceived(const std::string& senderName, const std::string& message) {
        std::cout << "[WhisperAlert] Received PM from: " << senderName << " | Content: " << message << std::endl;

        // Check if GM/Admin
        if (senderName.find("[GM]") != std::string::npos || senderName.find("[SGM]") != std::string::npos || senderName.find("[TL]") != std::string::npos) {
            std::cout << "[!] CRITICAL: GM WHISPER DETECTED! Pausing Bot & Playing Alarm..." << std::endl;
            Beep(750, 1000); // Trigger Audio Safety Alarm
            return;
        }

        // Random Auto Response to regular players
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, m_autoResponses.size() - 1);

        std::string reply = m_autoResponses[dis(gen)];
        std::cout << "[+] Sending Humanized Auto-Reply: " << reply << std::endl;
    }
};

} // namespace GlobalEngine
