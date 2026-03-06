#include "systems/PhysicsSystem.h"
#include "entities/Player.h"
#include "core/Constants.h"
namespace EC {
void PhysicsSystem::update(Player& player, float dt) {
    applyGravity(player);
    clampVelocity(player);
    integratePosition(player);
    wrapHorizontal(player);
    (void)dt;
}
void PhysicsSystem::applyGravity(Player& player) {
    player.velocity.y += EC::GRAVITY;
}
void PhysicsSystem::clampVelocity(Player& player) {
    if (player.velocity.y > EC::TERMINAL_VELOCITY)
        player.velocity.y = EC::TERMINAL_VELOCITY;
}
void PhysicsSystem::integratePosition(Player& player) {
    player.bounds.x += player.velocity.x;
    player.bounds.y += player.velocity.y;
}
void PhysicsSystem::wrapHorizontal(Player& player) {
    if (player.bounds.right() < 0)
        player.bounds.x = (float)EC::SCREEN_WIDTH;
    if (player.bounds.x > EC::SCREEN_WIDTH)
        player.bounds.x = -player.bounds.w;
}
} // namespace EC
