#pragma once
// ═══════════════════════════════════════════════════════════════
//  AchievementManager.h  — no SDL includes needed
// ═══════════════════════════════════════════════════════════════
#include "core/Types.h"
#include <vector>
#include <functional>
#include <string>

namespace EC {
struct Achievement {
    int         id;
    std::string name;
    std::string description;
    int         rewardStars;
    bool        unlocked = false;
    std::function<bool(const PlayerStats&, const SaveData&)> condition;
};

class AchievementManager {
public:
    AchievementManager();
    void check(PlayerStats& stats, SaveData& save);
    const std::vector<Achievement>& all() const { return m_achievements; }
private:
    std::vector<Achievement> m_achievements;
    void registerAll();
};
} // namespace EC
