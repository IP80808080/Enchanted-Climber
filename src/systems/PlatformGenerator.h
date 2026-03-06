#pragma once
// SDL3 core → <SDL3/SDL.h>
#include <SDL3/SDL.h>
#include "core/Types.h"
#include "core/Constants.h"
#include "entities/Platform.h"
#include <array>
#include <random>
namespace EC {
class PlatformGenerator {
public:
    void init  (ThemeID theme);
    void reset ();
    void update(std::array<Platform, EC::PLATFORM_POOL_SIZE>& pool,
                float cameraY, float difficulty);
private:
    ThemeID  m_theme = ThemeID::FAIRY_GARDEN;
    float    m_nextY = 0.0f;
    float    m_lastX = 0.0f;
    std::mt19937 m_rng;
    Platform& getInactive(std::array<Platform, EC::PLATFORM_POOL_SIZE>& pool);
    float     randomWidth();
    float     randomGap  (float difficulty);
    PlatformType randomType();
    bool      isReachable(float fromX, float fromY, float toX, float toY);
};
} // namespace EC
