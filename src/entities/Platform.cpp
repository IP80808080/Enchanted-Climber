// ═══════════════════════════════════════════════════════════════
//  Platform.cpp — renders using generated platform PNGs
//
//  Asset map:
//    PlatformType::NORMAL   → platform_<theme>.png  (theme-colored)
//    PlatformType::MOVING   → platform_moving.png   (blue shimmer)
//    PlatformType::BREAKING → platform_breaking.png (cracked)
//    PlatformType::BOUNCY   → platform_bouncy.png   (green dots)
//    PlatformType::SPECIAL  → platform_special.png  (gold glowing)
//
//  Falls back to solid colored rect if texture not loaded.
// ═══════════════════════════════════════════════════════════════
#include "entities/Platform.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "managers/AssetManager.h"
#include "managers/ThemeManager.h"
#include <SDL3_image/SDL_image.h>
#include <cmath>

namespace EC {

    void Platform::reset(float x, float y, float w, PlatformType t) {
        bounds = { x, y, w, EC::PLATFORM_HEIGHT };
        type = t;
        active = true;
        broken = false;
        landCount = 0;
        originX = x;
        m_animTimer = 0.0f;
    }

    void Platform::update(float dt) {
        if (!active) return;
        m_animTimer += dt;

        if (type == PlatformType::MOVING) {
            bounds.x += moveDir * moveSpeed;
            if (bounds.x < 0 || bounds.right() > EC::SCREEN_WIDTH)
                moveDir = -moveDir;
        }
        if (type == PlatformType::BREAKING && broken) {
            // Sink after breaking
            bounds.y += 60.0f * dt;
            if (bounds.y > 2000.0f) active = false;
        }
    }

    void Platform::render(SDL_Renderer* r, float cameraY) {
        if (!active) return;

        float screenY = bounds.y - cameraY;
        if (screenY > EC::SCREEN_HEIGHT + 20 || screenY < -EC::PLATFORM_HEIGHT - 20)
            return;

        SDL_FRect dst = { bounds.x, screenY, bounds.w, bounds.h };

        // ── Choose texture based on type ───────────────────────────────
        const std::string SP = EC::SPRITE_PATH + "platforms/";
        std::string texPath;

        switch (type) {
        case PlatformType::MOVING:
            texPath = SP + "platform_moving.png";
            break;
        case PlatformType::BREAKING:
            texPath = SP + "platform_breaking.png";
            break;
        case PlatformType::BOUNCY:
            texPath = SP + "platform_bouncy.png";
            break;
        case PlatformType::SPECIAL:
            texPath = SP + "platform_special.png";
            break;
        case PlatformType::NORMAL:
        default:
            // Use the theme-specific colored platform
            switch (theme) {
            case ThemeID::FAIRY_GARDEN:    texPath = SP + "platform_fairy.png";   break;
            case ThemeID::MERMAID_LAGOON:  texPath = SP + "platform_mermaid.png"; break;
            case ThemeID::PRINCESS_CASTLE: texPath = SP + "platform_castle.png";  break;
            case ThemeID::WITCH_WONDERLAND:texPath = SP + "platform_witch.png";   break;
            case ThemeID::SWEET_TREATS:    texPath = SP + "platform_candy.png";   break;
            default:                       texPath = SP + "platform_normal.png";  break;
            }
            break;
        }

        SDL_Texture* tex = Game::instance().assets().getTexture(texPath);

        if (tex) {
            // Breaking: flash alpha before fully breaking
            if (type == PlatformType::BREAKING && landCount >= 1) {
                float flash = sinf(m_animTimer * 20.0f) * 0.4f + 0.6f;
                SDL_SetTextureAlphaMod(tex, (Uint8)(255 * flash));
            }
            else {
                SDL_SetTextureAlphaMod(tex, 255);
            }

            // Stretch texture to platform width
            SDL_RenderTexture(r, tex, nullptr, &dst);

            // Moving platform: blue shimmer overlay
            if (type == PlatformType::MOVING) {
                float shimmer = sinf(m_animTimer * 4.0f) * 0.3f + 0.7f;
                SDL_SetRenderDrawColor(r, 100, 180, 255, (Uint8)(30 * shimmer));
                SDL_RenderFillRect(r, &dst);
            }

            // Special platform: gold sparkle overlay
            if (type == PlatformType::SPECIAL) {
                float pulse = sinf(m_animTimer * 6.0f) * 0.4f + 0.6f;
                SDL_SetRenderDrawColor(r, 255, 220, 50, (Uint8)(40 * pulse));
                SDL_RenderFillRect(r, &dst);
            }

            // Bouncy: green bounce indicator
            if (type == PlatformType::BOUNCY) {
                float bounce = sinf(m_animTimer * 5.0f) * 0.5f + 0.5f;
                SDL_SetRenderDrawColor(r, 100, 255, 100, (Uint8)(35 * bounce));
                SDL_RenderFillRect(r, &dst);
            }

        }
        else {
            // Fallback: colored rect if texture missing
            const ThemeData& td = Game::instance().themes().current();
            SDL_Color col;
            switch (type) {
            case PlatformType::MOVING:   col = { 100,180,255,220 }; break;
            case PlatformType::BREAKING: col = { 200,140,80,200 };  break;
            case PlatformType::BOUNCY:   col = { 100,220,100,220 }; break;
            case PlatformType::SPECIAL:  col = { 255,210,50,220 };  break;
            default:
                col = { td.platformColor.r, td.platformColor.g,
                        td.platformColor.b, td.platformColor.a };
                break;
            }
            SDL_SetRenderDrawColor(r, col.r, col.g, col.b, col.a);
            SDL_RenderFillRect(r, &dst);

            // Top highlight
            SDL_SetRenderDrawColor(r, 255, 255, 255, 60);
            SDL_FRect top = { dst.x + 2, dst.y + 1, dst.w - 4, 3 };
            SDL_RenderFillRect(r, &top);
        }
    }

} // namespace EC