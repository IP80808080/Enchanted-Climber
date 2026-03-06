#pragma once
// SDL3 core → <SDL3/SDL.h>
#include <SDL3/SDL.h>
#include "core/Types.h"
namespace EC {
class Player;
class PhysicsSystem {
public:
    void update(Player& player, float dt);
private:
    void applyGravity      (Player& player);
    void clampVelocity     (Player& player);
    void integratePosition (Player& player);
    void wrapHorizontal    (Player& player);
};
} // namespace EC
