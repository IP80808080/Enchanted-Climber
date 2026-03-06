#pragma once
// ═══════════════════════════════════════════════════════════════
//  CollisionSystem.h
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include <array>
#include <cstddef>
#include "core/Types.h"
#include "core/Constants.h"
#include "entities/Platform.h"

namespace EC {
    // Forward declare player, fully include types for stats and array
    class Player;

    class CollisionSystem {
    public:
        // Use full qualification and std::size_t for template parameters
        bool checkPlayerPlatforms(Player& player,
            std::array<Platform, static_cast<std::size_t>(EC::PLATFORM_POOL_SIZE)>& platforms,
            PlayerStats& stats);
    };
} // namespace EC
