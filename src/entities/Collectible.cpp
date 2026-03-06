// ═══════════════════════════════════════════════════════════════
//  Collectible.cpp — texture-based rendering
//
//  Asset map (matches your assets/sprites/particles/ folder):
//    STAR   → sprites/particles/star.png
//    HEART  → sprites/particles/heart.png
//    CANDY  → sprites/particles/star.png  (gold tint)
//    BUBBLE → sprites/particles/bubble.png
//
//  All original animations preserved:
//    - bobbing (sinf bobTimer)
//    - spinning (SDL_RenderTextureRotated with spinAngle)
//    - glow pulse (alpha-modulated glow rect behind sprite)
//    - beat scale on HEART
//
//  Falls back to the original procedural drawing if PNG missing.
// ═══════════════════════════════════════════════════════════════
#include "entities/Collectible.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "managers/AssetManager.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cmath>

namespace EC {

    // ── Texture path per collectible type ─────────────────────────
    static const char* collectTexPath(CollectibleType t) {
        switch (t) {
        case CollectibleType::STAR:   return "assets/sprites/particles/star.png";
        case CollectibleType::HEART:  return "assets/sprites/particles/heart.png";
        case CollectibleType::CANDY:  return "assets/sprites/particles/star.png";   // gold-tinted
        default:                      return "assets/sprites/particles/bubble.png";
        }
    }

    // ── init ──────────────────────────────────────────────────────
    void Collectible::init(CollectibleType t, float x, float y) {
        bounds = { x - 10, y, 20, 20 };
        collectType = t;
        active = true;
        collected = false;
        bobTimer = 0.0f;
        spinAngle = 0.0f;
        value = (t == CollectibleType::STAR) ? 1 : 0;
    }

    void Collectible::init(float x, float y, CollectibleType t) {
        init(t, x, y);
    }

    // ── update ────────────────────────────────────────────────────
    void Collectible::update(float dt) {
        if (!active) return;
        bobTimer += dt;
        spinAngle += dt * 180.0f;                     // degrees/sec
        bounds.y += sinf(bobTimer * 3.5f) * 0.3f;   // gentle bob
    }

    // ── render ────────────────────────────────────────────────────
    void Collectible::render(SDL_Renderer* r, float cameraY) {
        if (!active || collected) return;

        float sy = bounds.y - cameraY;
        float sz = bounds.w;
        float cx = bounds.x + sz * 0.5f;
        float cy = sy + sz * 0.5f;

        // Cull off-screen
        if (sy > EC::SCREEN_HEIGHT + 40 || sy < -sz - 40) return;

        SDL_Texture* tex = Game::instance().assets().getTexture(collectTexPath(collectType));

        // ── Glow pulse behind sprite ───────────────────────────────
        float gp = sinf(bobTimer * 4.0f) * 0.5f + 0.5f;

        if (collectType == CollectibleType::STAR || collectType == CollectibleType::CANDY) {
            // Gold glow
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
            for (int i = 3; i >= 1; i--) {
                float gs = sz * (0.8f + i * 0.4f);
                SDL_SetRenderDrawColor(r, 255, 220, 50, (Uint8)(30 * gp * (1.0f / i)));
                SDL_FRect gRect = { cx - gs * 0.5f, cy - gs * 0.5f, gs, gs };
                SDL_RenderFillRect(r, &gRect);
            }
        }
        else if (collectType == CollectibleType::HEART) {
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
            for (int i = 3; i >= 1; i--) {
                float gs = sz * (0.8f + i * 0.4f);
                SDL_SetRenderDrawColor(r, 255, 100, 150, (Uint8)(30 * gp * (1.0f / i)));
                SDL_FRect gRect = { cx - gs * 0.5f, cy - gs * 0.5f, gs, gs };
                SDL_RenderFillRect(r, &gRect);
            }
        }
        else {
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(r, 150, 220, 255, (Uint8)(25 * gp));
            SDL_FRect gRect = { cx - sz, cy - sz, sz * 2, sz * 2 };
            SDL_RenderFillRect(r, &gRect);
        }

        // ── Draw sprite ───────────────────────────────────────────
        if (tex) {
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

            // HEART: beat scale effect
            float drawSz = sz;
            if (collectType == CollectibleType::HEART) {
                float beat = 1.0f + sinf(bobTimer * 4.0f) * 0.18f;
                drawSz = sz * beat;
            }

            // CANDY: tint gold-orange
            if (collectType == CollectibleType::CANDY) {
                SDL_SetTextureColorMod(tex, 255, 180, 60);
            }
            else {
                SDL_SetTextureColorMod(tex, 255, 255, 255);
            }
            SDL_SetTextureAlphaMod(tex, 255);

            SDL_FRect dst = {
                cx - drawSz * 0.5f,
                cy - drawSz * 0.5f,
                drawSz, drawSz
            };

            // STAR / CANDY: spin
            double angle = (collectType == CollectibleType::STAR ||
                collectType == CollectibleType::CANDY)
                ? (double)spinAngle : 0.0;

            SDL_RenderTextureRotated(r, tex, nullptr, &dst,
                angle, nullptr, SDL_FLIP_NONE);

            // Reset color mod so cached texture isn't affected next frame
            SDL_SetTextureColorMod(tex, 255, 255, 255);

        }
        else {
            // ── Fallback: original procedural drawing ──────────────
            if (collectType == CollectibleType::STAR) {
                float oR = sz * 0.5f, iR = oR * 0.4f;
                float ba = spinAngle * SDL_PI_F / 180.0f - SDL_PI_F / 2.0f;
                SDL_SetRenderDrawColor(r, 255, 220, 50, 255);
                for (int i = 0; i < 5; i++) {
                    float a1 = ba + i * 2.0f * SDL_PI_F / 5.0f;
                    float a2 = a1 + SDL_PI_F / 5.0f;
                    float ox1 = cx + oR * cosf(a1), oy1 = cy + oR * sinf(a1);
                    float ix2 = cx + iR * cosf(a2), iy2 = cy + iR * sinf(a2);
                    SDL_RenderLine(r, ox1, oy1, ix2, iy2);
                    SDL_RenderLine(r, ix2, iy2, cx, cy);
                    SDL_RenderLine(r, cx, cy, ox1, oy1);
                }
                SDL_SetRenderDrawColor(r, 255, 255, 255, 220);
                SDL_FRect shine = { cx - sz * 0.15f, cy - sz * 0.3f, sz * 0.25f, sz * 0.15f };
                SDL_RenderFillRect(r, &shine);

            }
            else if (collectType == CollectibleType::HEART) {
                float beat = 1.0f + sinf(bobTimer * 4.0f) * 0.15f;
                float hw = sz * beat * 0.45f;
                SDL_SetRenderDrawColor(r, 255, 120, 160, 230);
                SDL_FRect body = { cx - hw, cy - hw * 0.5f, hw * 2,  hw * 1.5f };
                SDL_FRect tL = { cx - hw, cy - hw,      hw,    hw };
                SDL_FRect tR = { cx,      cy - hw,      hw,    hw };
                SDL_RenderFillRect(r, &body);
                SDL_RenderFillRect(r, &tL);
                SDL_RenderFillRect(r, &tR);

            }
            else if (collectType == CollectibleType::CANDY) {
                SDL_SetRenderDrawColor(r, 255, 160, 50, 230);
                SDL_FRect coin = { bounds.x, sy, sz, sz };
                SDL_RenderFillRect(r, &coin);
                SDL_SetRenderDrawColor(r, 255, 220, 120, 180);
                SDL_FRect stripe = { bounds.x + 2, sy + sz * 0.3f, sz - 4, sz * 0.3f };
                SDL_RenderFillRect(r, &stripe);

            }
            else {
                float bR = sz * 0.5f;
                SDL_SetRenderDrawColor(r, 180, 230, 255, 100);
                SDL_FRect bub = { cx - bR, cy - bR, bR * 2, bR * 2 };
                SDL_RenderFillRect(r, &bub);
                SDL_SetRenderDrawColor(r, 255, 255, 255, 150);
                SDL_FRect sh = { cx - bR * 0.4f, cy - bR * 0.5f, bR * 0.5f, bR * 0.3f };
                SDL_RenderFillRect(r, &sh);
            }
        }
    }

} // namespace EC