#pragma once
#include <SDL3/SDL.h>
#include "states/GameState.h"
#include "ui/Button.h"
#include <vector>
namespace EC {
class PausedState : public GameState {
public:
    void onEnter()  override;
    void onExit()   override;
    void onPause()  override;
    void onResume() override;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render()         override;
private:
    std::vector<Button> m_buttons;
    float m_timer = 0.0f;
};
} // namespace EC
