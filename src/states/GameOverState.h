#pragma once
#include <SDL3/SDL.h>
#include "states/GameState.h"
#include "ui/Button.h"
#include "core/Types.h"
#include <vector>
namespace EC {
class GameOverState : public GameState {
public:
    explicit GameOverState(const PlayerStats& stats);
    void onEnter()  override;
    void onExit()   override;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render()         override;
private:
    PlayerStats         m_stats;
    std::vector<Button> m_buttons;
    bool                m_newHighScore = false;
    float               m_timer        = 0.0f;
};
} // namespace EC
