#include "systems/ScoreSystem.h"
#include "core/Constants.h"
#include <SDL3/SDL.h>
#include <algorithm>
namespace EC {
void ScoreSystem::reset() { m_lowestY = 99999.0f; }

void ScoreSystem::start(float startY) {
    m_lowestY = startY;
}

void ScoreSystem::update(PlayerStats& stats, float playerY, float) {
    if (playerY < m_lowestY) {
        float diff = m_lowestY - playerY;
        stats.height += diff;
        stats.score  += (int)(diff * EC::SCORE_PER_PIXEL * stats.scoreMultiplier);
        m_lowestY = playerY;
    }
}

void ScoreSystem::onLand(PlayerStats& stats) {
    stats.streakCount++;
    if (stats.streakCount % EC::STREAK_BONUS_STEP == 0) {
        stats.scoreMultiplier = std::min(
            stats.scoreMultiplier + EC::STREAK_BONUS,
            EC::STREAK_BONUS_MAX);
    }
}

void ScoreSystem::addPlatformScore(PlayerStats& stats) {
    onLand(stats);
}

void ScoreSystem::onCollect(PlayerStats& stats, int value) {
    stats.stars += value;
    stats.score += EC::SCORE_PER_STAR * value;
}

void ScoreSystem::onSpecialPlatform(PlayerStats& stats) {
    stats.score += (int)(EC::SCORE_PER_SPECIAL * stats.scoreMultiplier);
}
} // namespace EC
