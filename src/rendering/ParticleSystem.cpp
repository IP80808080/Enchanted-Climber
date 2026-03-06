// ═══════════════════════════════════════════════════════════════
//  ParticleSystem.cpp — texture-based particles
// ═══════════════════════════════════════════════════════════════
#include "rendering/ParticleSystem.h"
#include "core/Game.h"
#include "core/Constants.h"

// ── Full definitions required to call methods on these types ───
#include "managers/AssetManager.h"   // Game::assets().getTexture()
#include "managers/ThemeManager.h"   // ThemeID enum used in themeColor()
#include "rendering/UIRenderer.h"    // (forward compat, safe to include)

#include <SDL3/SDL.h>
#include <cmath>
#include <cstdlib>

namespace EC {

    static const char* particleTexPath(ParticleType t) {
        switch (t) {
        case ParticleType::SPARKLE:    return "assets/sprites/particles/particle_sparkle.png";
        case ParticleType::PETAL:      return "assets/sprites/particles/particle_petal.png";
        case ParticleType::BUBBLE:     return "assets/sprites/particles/particle_bubble.png";
        case ParticleType::HEART:      return "assets/sprites/particles/particle_heart.png";
        case ParticleType::STAR:       return "assets/sprites/particles/particle_star.png";
        case ParticleType::CONFETTI:   return "assets/sprites/particles/particle_confetti.png";
        case ParticleType::CANDY:      return "assets/sprites/particles/particle_candy.png";
        case ParticleType::MAGIC_DUST: return "assets/sprites/particles/particle_magic_dust.png";
        default:                       return "assets/sprites/particles/particle_sparkle.png";
        }
    }

    void ParticleSystem::spawnBurst(Vec2 pos, int count, ParticleType type,
        Color color, float speed, float lifetime) {
        for (int i = 0; i < count; i++) {
            Particle& p = getFree();
            p.active = true;
            p.position = pos;
            p.type = type;
            p.color = color;
            p.maxLifetime = lifetime;
            p.lifetime = lifetime;
            p.size = 6.0f + (rand() % 8);
            p.rotSpeed = ((rand() % 200) - 100) * 0.015f;
            p.rotation = (float)(rand() % 360);

            float angle = ((float)(rand() % 360)) * 3.14159f / 180.0f;
            float spd = speed * (0.4f + (rand() % 100) * 0.012f);

            if (type == ParticleType::BUBBLE)
                p.velocity = { cosf(angle) * spd * 0.4f, -spd * (0.8f + (rand() % 100) * 0.005f) };
            else if (type == ParticleType::PETAL || type == ParticleType::CONFETTI)
                p.velocity = { cosf(angle) * spd, sinf(angle) * spd * 0.6f - spd * 0.2f };
            else
                p.velocity = { cosf(angle) * spd, sinf(angle) * spd - spd * 0.3f };
        }
    }

    void ParticleSystem::spawnJump(Vec2 pos, ThemeID theme) {
        spawnBurst(pos, EC::JUMP_PARTICLES, ParticleType::SPARKLE, themeColor(theme), 3.0f, 0.5f);
    }
    void ParticleSystem::spawnLand(Vec2 pos, ThemeID theme) {
        spawnBurst(pos, EC::LAND_PARTICLES, ParticleType::PETAL, themeColor(theme), 2.5f, 0.35f);
    }
    void ParticleSystem::spawnCollect(Vec2 pos, ThemeID theme) {
        spawnBurst(pos, EC::COLLECT_PARTICLES, ParticleType::STAR, { 255,220,50,255 }, 4.0f, 0.5f);
        spawnBurst(pos, 5, ParticleType::SPARKLE, themeColor(theme), 2.5f, 0.4f);
    }
    void ParticleSystem::spawnTransform(Vec2 pos, ThemeID theme) {
        spawnBurst(pos, EC::TRANSFORM_PARTICLES, ParticleType::MAGIC_DUST, themeColor(theme), 5.0f, 1.5f);
        spawnBurst(pos, 20, ParticleType::SPARKLE, { 255,255,200,255 }, 4.0f, 1.0f);
    }
    void ParticleSystem::spawnHighScore(Vec2 center) {
        spawnBurst(center, EC::HIGHSCORE_PARTICLES, ParticleType::CONFETTI, { 255,200,50,255 }, 6.0f, 2.0f);
        spawnBurst(center, 20, ParticleType::STAR, { 255,255,150,255 }, 5.0f, 1.5f);
    }

    void ParticleSystem::update(float dt) {
        for (auto& p : m_pool) {
            if (!p.active) continue;
            p.lifetime -= dt;
            if (p.lifetime <= 0.0f) { p.active = false; continue; }

            float gravity = (p.type == ParticleType::BUBBLE) ?
                -EC::PARTICLE_GRAVITY * 0.5f : EC::PARTICLE_GRAVITY;

            p.velocity.y += gravity;
            p.position += p.velocity;
            p.rotation += p.rotSpeed;

            float alpha = p.lifetime / p.maxLifetime;
            p.color.a = (Uint8)(255 * alpha * alpha);
        }
    }

    void ParticleSystem::render(SDL_Renderer* r) {
        for (const auto& p : m_pool) {
            if (!p.active) continue;

            // AssetManager is now fully defined — getTexture() resolves correctly
            SDL_Texture* tex = Game::instance().assets().getTexture(particleTexPath(p.type));
            float half = p.size * 0.5f;
            SDL_FRect dst = { p.position.x - half, p.position.y - half, p.size, p.size };

            if (tex) {
                SDL_SetTextureColorMod(tex, p.color.r, p.color.g, p.color.b);
                SDL_SetTextureAlphaMod(tex, p.color.a);
                SDL_RenderTextureRotated(r, tex, nullptr, &dst,
                    (double)p.rotation, nullptr, SDL_FLIP_NONE);
                SDL_SetTextureColorMod(tex, 255, 255, 255);
                SDL_SetTextureAlphaMod(tex, 255);
            }
            else {
                SDL_SetRenderDrawColor(r, p.color.r, p.color.g, p.color.b, p.color.a);
                SDL_RenderFillRect(r, &dst);
            }
        }
    }

    void ParticleSystem::renderWithCamera(SDL_Renderer* r, float cameraY) {
        for (const auto& p : m_pool) {
            if (!p.active) continue;

            SDL_Texture* tex = Game::instance().assets().getTexture(particleTexPath(p.type));
            float half = p.size * 0.5f;
            SDL_FRect dst = {
                p.position.x - half,
                p.position.y - half - cameraY,
                p.size, p.size
            };

            if (tex) {
                SDL_SetTextureColorMod(tex, p.color.r, p.color.g, p.color.b);
                SDL_SetTextureAlphaMod(tex, p.color.a);
                SDL_RenderTextureRotated(r, tex, nullptr, &dst,
                    (double)p.rotation, nullptr, SDL_FLIP_NONE);
                SDL_SetTextureColorMod(tex, 255, 255, 255);
                SDL_SetTextureAlphaMod(tex, 255);
            }
            else {
                SDL_SetRenderDrawColor(r, p.color.r, p.color.g, p.color.b, p.color.a);
                SDL_RenderFillRect(r, &dst);
            }
        }
    }

    void ParticleSystem::clear() {
        for (auto& p : m_pool) p.active = false;
    }

    Particle& ParticleSystem::getFree() {
        for (auto& p : m_pool) if (!p.active) return p;
        m_pool[0].active = false;
        return m_pool[0];
    }

    Color ParticleSystem::themeColor(ThemeID theme) {
        switch (theme) {
        case ThemeID::FAIRY_GARDEN:    return { 255,200,230,255 };
        case ThemeID::MERMAID_LAGOON:  return { 180,240,255,255 };
        case ThemeID::PRINCESS_CASTLE: return { 255,150,200,255 };
        case ThemeID::WITCH_WONDERLAND:return { 200,150,255,255 };
        case ThemeID::SWEET_TREATS:    return { 255,210,100,255 };
        default:                       return { 255,255,255,255 };
        }
    }

} // namespace EC