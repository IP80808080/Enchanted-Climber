// ═══════════════════════════════════════════════════════════════
//  Player.cpp — sprite animation from player_sheet.png
//
//  Spritesheet layout (48×48 per frame, 8 cols × 4 rows):
//    Row 0 (y=0):   IDLE  — 8 frames
//    Row 1 (y=48):  JUMP  — 8 frames
//    Row 2 (y=96):  FALL  — 8 frames
//    Row 3 (y=144): LAND  — 8 frames (first 3 used, rest padding)
//
//  The sheet was generated at 384×192 (8*48 × 4*48).
// ═══════════════════════════════════════════════════════════════
#include "entities/Player.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "managers/AssetManager.h"
#include <SDL3_image/SDL_image.h>
#include <cmath>

namespace EC {

    static constexpr int   FRAME_W = 48;
    static constexpr int   FRAME_H = 48;
    static constexpr int   SHEET_COLS = 8;

    // How many frames each anim state uses
    static constexpr int FRAMES_IDLE = 8;
    static constexpr int FRAMES_JUMP = 4;   // use first 4 of row 1
    static constexpr int FRAMES_FALL = 4;   // use first 4 of row 2
    static constexpr int FRAMES_LAND = 3;   // use first 3 of row 3

    // Row index in sheet for each state
    static constexpr int ROW_IDLE = 0;
    static constexpr int ROW_JUMP = 1;
    static constexpr int ROW_FALL = 2;
    static constexpr int ROW_LAND = 3;

    void Player::init(CharacterID id) {
        characterID = id;
        animState = AnimState::IDLE;
        facingRight = true;
        m_animTimer = 0.0f;
        m_landTimer = 0.0f;
        m_grounded = false;
        velocity = { 0.0f, 0.0f };
    }

    void Player::update(float dt) {
        m_animTimer += dt;
        if (m_landTimer > 0.0f) {
            m_landTimer -= dt;
            if (m_landTimer <= 0.0f && m_grounded)
                animState = AnimState::IDLE;
        }

        // Determine facing direction
        if (velocity.x > 0.1f)  facingRight = true;
        if (velocity.x < -0.1f) facingRight = false;

        // Anim state from physics
        if (!m_grounded) {
            animState = (velocity.y < 0.0f) ? AnimState::JUMPING : AnimState::FALLING;
        }
    }

    void Player::applyJump() {
        velocity.y = EC::JUMP_FORCE;
        m_grounded = false;
        animState = AnimState::JUMPING;
        m_animTimer = 0.0f;
    }

    void Player::land(float platformY) {
        bounds.y = platformY - bounds.h;
        velocity.y = 0.0f;
        m_grounded = true;
        animState = AnimState::LANDING;
        m_landTimer = 0.18f;
        m_animTimer = 0.0f;
    }

    void Player::render(SDL_Renderer* r, float cameraY) {
        const std::string sheetPath =
            EC::SPRITE_PATH + "characters/player_sheet.png";
        SDL_Texture* sheet = Game::instance().assets().getTexture(sheetPath);

        float screenY = bounds.y - cameraY;
        SDL_FRect dst = { bounds.x, screenY, bounds.w, bounds.h };

        if (sheet) {
            // Pick row & frame count for current state
            int row = ROW_IDLE;
            int frameCount = FRAMES_IDLE;
            float speed = EC::ANIM_FRAME_DURATION;

            switch (animState) {
            case AnimState::JUMPING:
                row = ROW_JUMP; frameCount = FRAMES_JUMP; speed = 0.10f; break;
            case AnimState::FALLING:
                row = ROW_FALL; frameCount = FRAMES_FALL; speed = 0.12f; break;
            case AnimState::LANDING:
                row = ROW_LAND; frameCount = FRAMES_LAND; speed = 0.06f; break;
            default:
                row = ROW_IDLE; frameCount = FRAMES_IDLE; speed = EC::ANIM_FRAME_DURATION;
                break;
            }

            int col = (int)(m_animTimer / speed) % frameCount;

            SDL_FRect src = {
                (float)(col * FRAME_W),
                (float)(row * FRAME_H),
                (float)FRAME_W,
                (float)FRAME_H
            };

            // Flip horizontally if facing left
            SDL_FlipMode flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            SDL_RenderTextureRotated(r, sheet, &src, &dst, 0.0, nullptr, flip);

            // Landing squash: slight scale-down height, widen
            if (animState == AnimState::LANDING) {
                float squash = 1.0f - (m_landTimer / 0.18f) * 0.15f;
                dst.h = bounds.h * squash;
                dst.y = screenY + (bounds.h - dst.h);
                dst.w = bounds.w * (2.0f - squash);
                dst.x = bounds.x - (dst.w - bounds.w) * 0.5f;
                SDL_RenderTextureRotated(r, sheet, &src, &dst, 0.0, nullptr, flip);
            }

        }
        else {
            // Fallback: colored rectangle with simple face
            SDL_SetRenderDrawColor(r, 255, 150, 200, 255);
            SDL_RenderFillRect(r, &dst);

            // Head
            SDL_SetRenderDrawColor(r, 255, 220, 200, 255);
            SDL_FRect head = { dst.x + dst.w * 0.2f, dst.y + 2, dst.w * 0.6f, dst.w * 0.6f };
            SDL_RenderFillRect(r, &head);

            // Eyes
            SDL_SetRenderDrawColor(r, 40, 40, 80, 255);
            SDL_FRect le = { dst.x + dst.w * 0.3f, dst.y + 8, 4, 4 };
            SDL_FRect re = { dst.x + dst.w * 0.6f, dst.y + 8, 4, 4 };
            SDL_RenderFillRect(r, &le);
            SDL_RenderFillRect(r, &re);
        }
    }

} // namespace EC