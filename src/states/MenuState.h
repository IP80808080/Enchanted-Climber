#pragma once
// ═══════════════════════════════════════════════════════════════
//  MenuState.h — World-class main menu
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "states/GameState.h"
#include "ui/Button.h"
#include <vector>

namespace EC {

    class MenuState : public GameState {
    public:
        void onEnter() override;
        void onExit() override;
        void handleEvent(const SDL_Event& e) override;
        void update(float dt) override;
        void render() override;

    private:
        void cycleTheme(int dir);

        std::vector<Button> m_buttons;
        float m_timer = 0.0f;
        float m_bgScrollY = 0.0f;
        float m_themeScrollTimer = 0.0f;
        float m_titleBob = 0.0f;
    };

} // namespace EC