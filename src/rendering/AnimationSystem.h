#pragma once
// ═══════════════════════════════════════════════════════════════
//  AnimationSystem.h
//  SDL3 core  → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "core/Types.h"
#include "core/Constants.h"
#include <string>

namespace EC {
class AnimationSystem {
public:
    void load    (SDL_Renderer* r, const std::string& spriteFolder);
    void setState(AnimState s);
    void update  (float dt);
    void render  (SDL_Renderer* r, const Rect& dest, bool flipH = false);

private:
    struct AnimClip {
        SDL_Texture* texture = nullptr;
        int frames  = 1;
        int frameW  = 48;
        int frameH  = 64;
    };
    AnimClip  m_clips[(int)AnimState::LANDING + 1];
    AnimState m_current    = AnimState::IDLE;
    int       m_frame      = 0;
    float     m_frameTimer = 0.0f;
};
} // namespace EC
