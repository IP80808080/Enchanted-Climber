#include "systems/CollisionSystem.h"
#include "entities/Player.h"
#include "entities/Platform.h"
#include "core/Constants.h"
#include <cstddef>

namespace EC {
    bool CollisionSystem::checkPlayerPlatforms(
            Player& player,
            std::array<Platform, static_cast<std::size_t>(EC::PLATFORM_POOL_SIZE)>& platforms,
            PlayerStats& stats) {
        
        if (player.velocity.y <= 0.0f) return false;

        for (auto& plat : platforms) {
            if (!plat.active || plat.broken) continue;

            float pBottom = player.bounds.bottom() - player.velocity.y;
            bool  wasAbove = pBottom <= plat.bounds.top() + EC::LAND_TOLERANCE;
            
            bool  overlapsX = player.bounds.right()  > plat.bounds.left() &&
                              player.bounds.left()   < plat.bounds.right();
            
            bool  overlapsY = player.bounds.bottom() >= plat.bounds.top() &&
                              player.bounds.top()    <  plat.bounds.bottom();

            if (wasAbove && overlapsX && overlapsY) {
                player.land(plat.bounds.top());
                
                stats.platformsLanded++;
                stats.streakCount++;
                plat.landCount++;

                if (plat.type == PlatformType::BREAKING)
                    plat.broken = true;

                return true;
            }
        }
        return false;
    }
} // namespace EC
