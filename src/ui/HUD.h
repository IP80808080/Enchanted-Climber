#pragma once
// ═══════════════════════════════════════════════════════════════
//  HUD.h
//  SDL3 core → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "core/Types.h"

namespace EC {
class HUD {
public:
    void render(SDL_Renderer* r, const PlayerStats& stats);
private:
    float m_starPulse = 0.0f;
};
} // namespace EC
