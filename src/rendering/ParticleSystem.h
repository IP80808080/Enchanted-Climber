#pragma once
// ═══════════════════════════════════════════════════════════════
//  ParticleSystem.h
//  SDL3 core → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "core/Types.h"
#include "core/Constants.h"
#include <array>

namespace EC {
class ParticleSystem {
public:
    void spawnJump     (Vec2 pos, ThemeID theme);
    void spawnLand     (Vec2 pos, ThemeID theme);
    void spawnCollect  (Vec2 pos, ThemeID theme);
    void spawnTransform(Vec2 pos, ThemeID theme);
    void spawnHighScore(Vec2 screenCenter);

    void update(float dt);
    void render           (SDL_Renderer* r);
    void renderWithCamera (SDL_Renderer* r, float cameraY);
    void clear();

private:
    std::array<Particle, PARTICLE_POOL_SIZE> m_pool;

    Particle& getFree();
    void      spawnBurst(Vec2 pos, int count, ParticleType type,
                         Color color, float speed, float lifetime);
    Color     themeColor(ThemeID theme);
};
} // namespace EC
