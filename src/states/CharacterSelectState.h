#pragma once
#include <SDL3/SDL.h>
#include "states/GameState.h"
#include "ui/Button.h"
#include <vector>

namespace EC {
    class CharacterSelectState : public GameState {
    public:
        void onEnter()  override;
        void onExit()   override;
        void handleEvent(const SDL_Event& e) override;
        void update(float dt) override;
        void render()         override;

    private:
        std::vector<Button> m_buttons;
        int   m_selected = 0;
        float m_timer    = 0.0f;
        void  selectCharacter();
    };
} // namespace EC
