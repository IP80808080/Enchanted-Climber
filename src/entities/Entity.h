#pragma once
// ═══════════════════════════════════════════════════════════════
//  Entity.h — base class for all game objects
//  SDL3 core → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "core/Types.h"

namespace EC {
class Entity {
public:
    virtual ~Entity() = default;
    Rect  bounds;
    Vec2  velocity;
    bool  active = true;

    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* r, float cameraY) = 0;

    float centerX() const { return bounds.centerX(); }
    float centerY() const { return bounds.centerY(); }
};
} // namespace EC
