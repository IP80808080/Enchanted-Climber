#pragma once
// ═══════════════════════════════════════════════════════════════
//  Platform.h
//  SDL3 core → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "entities/Entity.h"
#include "core/Types.h"

namespace EC {

    class Platform : public Entity {
    public:
        PlatformType type = PlatformType::NORMAL;
        ThemeID      theme = ThemeID::FAIRY_GARDEN;
        bool         broken = false;
        int          landCount = 0;
        float        originX = 0.0f;
        float        moveDir = 1.0f;
        float        moveSpeed = 1.5f;

        void reset(float x, float y, float w, PlatformType t);
        void update(float dt)                      override;
        void render(SDL_Renderer* r, float cameraY) override;

    private:
        float m_animTimer = 0.0f;
    };

} // namespace EC