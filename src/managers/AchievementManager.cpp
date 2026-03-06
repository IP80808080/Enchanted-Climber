// ═══════════════════════════════════════════════════════════════
//  AchievementManager.cpp
// ═══════════════════════════════════════════════════════════════

#include "managers/AchievementManager.h"
#include <iostream>

namespace EC {

AchievementManager::AchievementManager() {
    registerAll();
}

void AchievementManager::check(PlayerStats& stats, SaveData& save) {
    for (auto& a : m_achievements) {
        if (a.unlocked) continue;
        if (a.condition && a.condition(stats, save)) {
            a.unlocked = true;
            save.totalStars += a.rewardStars;
            std::cout << "[Achievement] Unlocked: " << a.name
                      << " (+" << a.rewardStars << " stars)\n";
        }
    }
}

void AchievementManager::registerAll() {
    m_achievements = {
        {1,  "First Flight",        "Reach 100m",                    50,  false,
         [](const PlayerStats& s, const SaveData&){ return s.height >= 1000.0f; }},
        {2,  "Cloud Hopper",        "Land on 100 platforms",         75,  false,
         [](const PlayerStats& s, const SaveData&){ return s.platformsLanded >= 100; }},
        {3,  "Star Collector",      "Collect 50 stars",              80,  false,
         [](const PlayerStats& s, const SaveData&){ return s.stars >= 50; }},
        {4,  "Royal Heights",       "Reach 1000m",                   150, false,
         [](const PlayerStats& s, const SaveData&){ return s.height >= 10000.0f; }},
        {5,  "On A Streak!",        "Land 20 platforms in a row",    150, false,
         [](const PlayerStats& s, const SaveData&){ return s.streakCount >= 20; }},
        {6,  "Perfectionist",       "100 platforms without missing", 500, false,
         [](const PlayerStats& s, const SaveData&){ return s.streakCount >= 100; }},
        {7,  "High Scorer",         "Reach 10,000 points",           250, false,
         [](const PlayerStats& s, const SaveData&){ return s.score >= 10000; }},
        {8,  "Star Rich",           "Accumulate 500 total stars",    300, false,
         [](const PlayerStats&,   const SaveData& d){ return d.totalStars >= 500; }},
        {9,  "Witch's Apprentice",  "Reach 2000m",                   200, false,
         [](const PlayerStats& s, const SaveData&){ return s.height >= 20000.0f; }},
        {10, "Enchanted Master",    "Unlock all characters",        1000, false,
         [](const PlayerStats&, const SaveData& d){
             for (int i = 0; i < (int)CharacterID::COUNT; i++)
                 if (!d.unlockedChars[i]) return false;
             return true;
         }},
    };
}

} // namespace EC
