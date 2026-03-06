#include "systems/PlatformGenerator.h"
#include "core/Constants.h"
#include <ctime>
namespace EC {
void PlatformGenerator::init(ThemeID theme) {
    m_theme = theme;
    m_rng.seed((unsigned)time(nullptr));
    reset();
}
void PlatformGenerator::reset() {
    // Start generating platforms above the initial spawn area (around 0.65 screen height)
    // 0.45f (approx 384) gives a nice first gap from the 0.65f (555) spawn platform.
    m_nextY = (float)EC::SCREEN_HEIGHT * 0.45f; 
    m_lastX = EC::SCREEN_WIDTH / 2.0f;
}
void PlatformGenerator::update(
        std::array<Platform, PLATFORM_POOL_SIZE>& pool,
        float cameraY, float difficulty) {
    // Generate platforms above camera view
    while (m_nextY > cameraY - 200.0f) {
        Platform& p = getInactive(pool);
        float w   = randomWidth();
        float x   = (float)(m_rng() % (int)(EC::SCREEN_WIDTH - w));
        p.reset(x, m_nextY, w, randomType());
        p.theme   = m_theme;
        m_lastX   = x + w / 2;
        m_nextY  -= randomGap(difficulty);
    }
    // Deactivate platforms far below camera
    for (auto& p : pool)
        if (p.active && p.bounds.y > cameraY + EC::SCREEN_HEIGHT + 200.0f)
            p.active = false;
}
Platform& PlatformGenerator::getInactive(
        std::array<Platform, PLATFORM_POOL_SIZE>& pool) {
    for (auto& p : pool) if (!p.active) return p;
    pool[0].active = false;
    return pool[0];
}
float PlatformGenerator::randomWidth() {
    std::uniform_real_distribution<float> d(
        EC::PLATFORM_MIN_WIDTH, EC::PLATFORM_MAX_WIDTH);
    return d(m_rng);
}
float PlatformGenerator::randomGap(float difficulty) {
    float maxGap = EC::PLATFORM_GAP_MAX + difficulty * 2.0f;
    std::uniform_real_distribution<float> d(EC::PLATFORM_GAP_MIN, maxGap);
    return d(m_rng);
}
PlatformType PlatformGenerator::randomType() {
    std::uniform_real_distribution<float> d(0.0f, 1.0f);
    float r = d(m_rng);
    if (r < EC::SPECIAL_PLATFORM_CHANCE * 0.5f) return PlatformType::BREAKING;
    if (r < EC::SPECIAL_PLATFORM_CHANCE)        return PlatformType::MOVING;
    return PlatformType::NORMAL;
}
bool PlatformGenerator::isReachable(float, float, float, float) { return true; }
} // namespace EC
