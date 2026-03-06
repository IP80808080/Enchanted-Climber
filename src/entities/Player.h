#pragma once
// ═══════════════════════════════════════════════════════════════
//  Player.h
//  SDL3 core → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "entities/Entity.h"
#include "core/Types.h"

namespace EC {

    class Player : public Entity {
    public:
        CharacterID characterID = CharacterID::LILY_FAIRY;
        AnimState   animState = AnimState::IDLE;
        bool        facingRight = true;

        void init(CharacterID id);
        void update(float dt)                      override;
        void render(SDL_Renderer* r, float cameraY) override;
        void applyJump();
        void land(float platformY);
        bool isGrounded() const { return m_grounded; }

    private:
        bool  m_grounded = false;
        float m_animTimer = 0.0f;
        float m_landTimer = 0.0f;
    };

} // namespace EC