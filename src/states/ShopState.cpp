// ═══════════════════════════════════════════════════════════════
//  ShopState.cpp — Magic shop for unlocking themes & characters
// ═══════════════════════════════════════════════════════════════
#include "states/ShopState.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "rendering/UIRenderer.h"
#include "managers/AudioManager.h"
#include "managers/ThemeManager.h"
#include "rendering/ParticleSystem.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <string>

namespace EC {

void ShopState::onEnter() {
    m_timer    = 0.0f;
    m_selected = 0;
    buildItems();
}

void ShopState::onExit() {
    m_items.clear();
}

void ShopState::buildItems() {
    auto& save = Game::instance().saveData();
    m_items.clear();

    // Theme unlocks
    static const char* themeN[] = {
        "Fairy Garden", "Mermaid Lagoon", "Princess Castle",
        "Witch's Wonderland", "Sweet Treats"
    };
    static const int themeC[] = { 0, 200, 400, 600, 800 };

    for (int i = 0; i < static_cast<int>(ThemeID::COUNT); i++) {
        Item item;
        item.label     = themeN[i] + std::string(" Theme");
        item.cost      = themeC[i];
        item.unlocked  = save.unlockedThemes[i];
        item.themeIdx  = i;
        item.charIdx   = -1;
        m_items.push_back(item);
    }

    // Character unlocks
    struct { int idx; int cost; const char* name; } charSelect[] = {
        { static_cast<int>(CharacterID::MARINA_MERMAID),    150, "Marina Mermaid" },
        { static_cast<int>(CharacterID::ROSALIND_PRINCESS), 300, "Rosalind Princess" },
        { static_cast<int>(CharacterID::CHLOE_BAKER),       400, "Chloe the Baker" },
        { static_cast<int>(CharacterID::AURORA_BUTTERFLY),  600, "Aurora Butterfly" },
    };
    for (auto& ci : charSelect) {
        Item item;
        item.label    = ci.name + std::string(" Character");
        item.cost     = ci.cost;
        item.unlocked = save.unlockedChars[ci.idx];
        item.themeIdx = -1;
        item.charIdx  = ci.idx;
        m_items.push_back(item);
    }
}

void ShopState::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_EVENT_KEY_DOWN) {
        switch (e.key.scancode) {
            case SDL_SCANCODE_UP: case SDL_SCANCODE_W:
                m_selected = (m_selected - 1 + static_cast<int>(m_items.size())) %
                             static_cast<int>(m_items.size());
                Game::instance().audio().playSFX("hover");
                break;
            case SDL_SCANCODE_DOWN: case SDL_SCANCODE_S:
                m_selected = (m_selected + 1) % static_cast<int>(m_items.size());
                Game::instance().audio().playSFX("hover");
                break;
            case SDL_SCANCODE_RETURN: case SDL_SCANCODE_SPACE:
                tryPurchase();
                break;
            case SDL_SCANCODE_ESCAPE:
                Game::instance().popState();
                break;
            default: break;
        }
    }
}

void ShopState::tryPurchase() {
    if (m_items.empty()) return;
    if (m_selected < 0 || m_selected >= static_cast<int>(m_items.size())) return;
    
    auto& item = m_items[m_selected];
    if (item.unlocked) return;

    Game& g = Game::instance();
    auto& saveMgr = g.saveData();

    if (saveMgr.totalStars >= item.cost) {
        saveMgr.totalStars -= item.cost;
        item.unlocked = true;

        if (item.themeIdx >= 0 && item.themeIdx < static_cast<int>(ThemeID::COUNT))
            saveMgr.unlockedThemes[item.themeIdx] = true;
        
        if (item.charIdx >= 0 && item.charIdx < static_cast<int>(CharacterID::COUNT))
            saveMgr.unlockedChars[item.charIdx] = true;

        g.audio().playSFX("unlock");
        g.particles().spawnHighScore(
            { static_cast<float>(EC::SCREEN_WIDTH) / 2.0f, static_cast<float>(EC::SCREEN_HEIGHT) / 2.0f });
    } else {
        g.audio().playSFX("warning");
    }
}

void ShopState::update(float dt) {
    m_timer += dt;
}

void ShopState::render() {
    Game& g = Game::instance();
    SDL_Renderer* r = g.getRenderer();
    const ThemeData& currentT = g.themes().current();

    // Background
    g.ui().drawAnimatedBackground(currentT, m_timer * 0.3f);

    // Title
    g.ui().drawTextWithShadow("MAGIC SHOP",
        EC::SCREEN_WIDTH / 2, 18,
        EC::FONT_SIZE_LARGE, {255, 220, 50, 255}, true);

    // Stars
    auto& saveRec = g.saveData();
    g.ui().drawText("Stars: " + std::to_string(saveRec.totalStars),
        EC::SCREEN_WIDTH - 10, 18, EC::FONT_SIZE_MEDIUM,
        {255, 215, 50, 230}, false);

    SDL_SetRenderDrawColor(r, currentT.accentColor.r, currentT.accentColor.g, currentT.accentColor.b, 180);
    SDL_FRect dividerRect = { 20.0f, 64.0f, static_cast<float>(EC::SCREEN_WIDTH) - 40.0f, 2.0f };
    SDL_RenderFillRect(r, &dividerRect);

    // List layout
    float listY = 82.0f;
    float rowH  = 60.0f;
    float panelPosX = 20.0f;
    float panelWidthX = static_cast<float>(EC::SCREEN_WIDTH) - 40.0f;

    int maxV = static_cast<int>((EC::SCREEN_HEIGHT - 170) / rowH);
    int sOffset = std::max(0, m_selected - maxV / 2);

    // List container (Rounded)
    Rect listBg = { 15.0f, listY - 5.0f, panelWidthX + 10.0f, static_cast<float>(EC::SCREEN_HEIGHT - 210) };
    g.ui().drawRoundedRectFilled(listBg, 20.0f, { 0, 0, 0, 60 });

    for (int i = sOffset; i < static_cast<int>(m_items.size()) && i < sOffset + maxV; i++) {
        const auto& item = m_items[i];
        float currentY = listY + (i - sOffset) * rowH;
        bool isS = (i == m_selected);

        Rect rowRect = { panelPosX, currentY, panelWidthX, rowH - 6.0f };
        Color rowFill = isS ? 
            Color{ currentT.accentColor.r, currentT.accentColor.g, currentT.accentColor.b, 60 } :
            Color{ 25, 20, 50, 140 };
        
        g.ui().drawRoundedRectFilled(rowRect, 14.0f, rowFill);

        if (isS) {
            float pulseS = sinf(m_timer * 4.5f) * 0.2f + 0.8f;
            g.ui().drawRoundedRectFilled(rowRect, 14.0f, { 255, 255, 255, (Uint8)(30 * pulseS) });
        }

        Color labelColorText = item.unlocked ? Color{120, 255, 180, 230} : Color{255, 255, 255, 240};
        g.ui().drawText((item.unlocked ? "✦ " : "  ") + item.label,
            static_cast<int>(panelPosX + 18.0f), static_cast<int>(currentY + 14.0f),
            EC::FONT_SIZE_MEDIUM, labelColorText);

        if (!item.unlocked) {
            bool canAff = saveRec.totalStars >= item.cost;
            Color costC = canAff ? Color{255, 220, 50, 255} : Color{255, 100, 100, 230};
            g.ui().drawText(std::to_string(item.cost) + " *",
                static_cast<int>(panelPosX + panelWidthX - 90.0f), static_cast<int>(currentY + 14.0f),
                EC::FONT_SIZE_MEDIUM, costC);
        }
        else {
            g.ui().drawText("UNLOCKED", static_cast<int>(panelPosX + panelWidthX - 100.0f),
                           static_cast<int>(currentY + 14.0f), EC::FONT_SIZE_SMALL,
                           {100, 255, 150, 180});
        }
    }

    // Hint
    if (!m_items.empty() && !m_items[m_selected].unlocked) {
        g.ui().drawText("ENTER to purchase", EC::SCREEN_WIDTH / 2, EC::SCREEN_HEIGHT - 60, EC::FONT_SIZE_SMALL, {255, 255, 200, 180}, true);
    }
    g.ui().drawText("ESC to go back", EC::SCREEN_WIDTH / 2, EC::SCREEN_HEIGHT - 26, EC::FONT_SIZE_SMALL, {180, 180, 200, 130}, true);

    // Particles
    g.particles().render(r);
}

} // namespace EC
