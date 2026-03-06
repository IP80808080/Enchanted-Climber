#pragma once
// ═══════════════════════════════════════════════════════════════
//  GameState.h — abstract base for every game screen
//  SDL3 core → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>

namespace EC {
    class GameState {
    public:
        virtual ~GameState() = default;
        virtual void onEnter() {}
        virtual void onExit() {}
        virtual void onPause() {}
        virtual void onResume() {}
        virtual void handleEvent(const SDL_Event& event) { (void)event; }
        virtual void update(float dt) = 0;
        virtual void render() = 0;
    };
} // namespace EC