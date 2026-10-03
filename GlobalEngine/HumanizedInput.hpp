#pragma once
#include "IMemoryEngine.hpp"
#include <windows.h>
#include <random>
#include <thread>
#include <chrono>

namespace GlobalEngine {

class HumanizedInput {
private:
    IMemoryEngine* m_engine;
    std::mt19937 m_rng;

public:
    explicit HumanizedInput(IMemoryEngine* engine) : m_engine(engine), m_rng(std::random_device{}()) {}

    // Gaussian Randomized Sleep Delay to emulate human reaction variance
    void HumanSleep(int baseMs, int varianceMs) {
        std::normal_distribution<double> dist(baseMs, varianceMs / 2.0);
        int delay = static_cast<int>(dist(m_rng));
        if (delay < 10) delay = 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    // Perform Ring 0 / Win32 Stealth Key Click with Human Press-Release Timing
    void KeyClick(uint16_t virtualKeyCode) {
        if (m_engine && m_engine->IsKernelModeActive()) {
            m_engine->InjectStealthInput(1, 0, 0, 0, virtualKeyCode); // Key Down
            HumanSleep(45, 15);
            m_engine->InjectStealthInput(1, 1, 0, 0, virtualKeyCode); // Key Up
        } else {
            // Ring 3 Win32 Event Fallback
            keybd_event(static_cast<BYTE>(virtualKeyCode), 0, 0, 0);
            HumanSleep(45, 15);
            keybd_event(static_cast<BYTE>(virtualKeyCode), 0, KEYEVENTF_KEYUP, 0);
        }
    }

    // Bezier Curve Mouse Movement Trajectory Generator
    void MoveMouseHumanized(int startX, int startY, int endX, int endY, int steps = 20) {
        std::uniform_int_distribution<int> controlDist(-30, 30);
        int ctrlX = (startX + endX) / 2 + controlDist(m_rng);
        int ctrlY = (startY + endY) / 2 + controlDist(m_rng);

        for (int i = 0; i <= steps; ++i) {
            float t = static_cast<float>(i) / steps;
            float u = 1.0f - t;

            int x = static_cast<int>(u * u * startX + 2 * u * t * ctrlX + t * t * endX);
            int y = static_cast<int>(u * u * startY + 2 * u * t * ctrlY + t * t * endY);

            if (m_engine && m_engine->IsKernelModeActive()) {
                m_engine->InjectStealthInput(0, 0, static_cast<int16_t>(x - startX), static_cast<int16_t>(y - startY), 0);
            } else {
                SetCursorPos(x, y);
            }
            HumanSleep(10, 3);
        }
    }
};

} // namespace GlobalEngine
