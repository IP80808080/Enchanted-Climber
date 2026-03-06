#pragma once
#include <SDL3/SDL.h>
#include "states/GameState.h"
#include "core/Types.h"
#include <vector>
#include <string>
namespace EC {
class ShopState : public GameState {
public:
    void onEnter()  override;
    void onExit()   override;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render()         override;

private:
    struct Item {
        std::string label;
        int         cost     = 0;
        bool        unlocked = false;
        int         themeIdx = -1;
        int         charIdx  = -1;
    };

    std::vector<Item> m_items;
    int   m_selected = 0;
    float m_timer    = 0.0f;

    void buildItems();
    void tryPurchase();
};
} // namespace EC
