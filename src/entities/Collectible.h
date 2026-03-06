#pragma once
// ═══════════════════════════════════════════════════════════════
//  Collectible.h
//  SDL3 core → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "entities/Entity.h"

namespace EC {

    enum class CollectibleType { STAR, CANDY, BUBBLE, HEART };

    class Collectible : public Entity {
    public:
        CollectibleType collectType = CollectibleType::STAR;
        bool            collected = false;
        int             value = 1;

        void init(CollectibleType t, float x, float y);
        void init(float x, float y, CollectibleType t);  // legacy
        void update(float dt)                      override;
        void render(SDL_Renderer* r, float cameraY) override;

    private:
        float bobTimer = 0.0f;
        float spinAngle = 0.0f;
    };

} // namespace EC