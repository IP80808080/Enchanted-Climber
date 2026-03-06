// ═══════════════════════════════════════════════════════════════
//  CharacterSelectState.cpp — Magical girl character picker
// ═══════════════════════════════════════════════════════════════
#include "states/CharacterSelectState.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "rendering/UIRenderer.h"
#include "managers/AudioManager.h"
#include "managers/ThemeManager.h"
#include "ui/Button.h"
#include <cmath>
#include <string>
#include <vector>

namespace EC {

// Character display info
struct CharInfo {
    const char* name;
    const char* ability;
    const char* unlockHint;
    SDL_Color   color;
};

static const CharInfo CHAR_INFO[static_cast<size_t>(CharacterID::COUNT)] = {
    { "Lily",     "Double Jump",        "Default",          {255, 180, 220, 255} },
    { "Marina",   "Swim Fall",          "Reach 1000m",      {100, 220, 230, 255} },
    { "Rosalind", "Royal Shield",       "Collect 50 stars", {220, 150, 255, 255} },
    { "Morgana",  "Teleport Dash",      "Reach 2000m",      {120,  50, 180, 255} },
    { "Chloe",    "Candy Platforms",    "Collect 100 stars",{255, 180, 130, 255} },
    { "Aurora",   "Flutter Fall",       "Play 10 hours",    {200, 255, 180, 255} },
    { "Stella",   "Night 2x Points",   "Reach 5000m",      {180, 180, 255, 255} },
    { "Luna",     "Moon Jump",          "Secret",           {255, 255, 255, 240} },
};

void CharacterSelectState::onEnter() {
    Game& g = Game::instance();
    m_timer     = 0.0f;
    m_selected  = static_cast<int>(g.saveData().activeCharacter);

    // Initialise select button
    float bw = 200.0f, bh = 54.0f;
    Button selectBtn;
    selectBtn.init(static_cast<float>(EC::SCREEN_WIDTH) / 2.0f - bw / 2.0f,
                   static_cast<float>(EC::SCREEN_HEIGHT) * 0.82f,
                   bw, bh, "SELECT HEROINE",
                   [this]() { selectCharacter(); });
    
    m_buttons.push_back(std::move(selectBtn));
}

void CharacterSelectState::onExit() {
    m_buttons.clear();
}

void CharacterSelectState::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_EVENT_KEY_DOWN) {
        switch (e.key.scancode) {
            case SDL_SCANCODE_LEFT: case SDL_SCANCODE_A:
                m_selected = (m_selected - 1 + static_cast<int>(CharacterID::COUNT)) %
                             static_cast<int>(CharacterID::COUNT);
                Game::instance().audio().playSFX("click");
                break;
            case SDL_SCANCODE_RIGHT: case SDL_SCANCODE_D:
                m_selected = (m_selected + 1) % static_cast<int>(CharacterID::COUNT);
                Game::instance().audio().playSFX("click");
                break;
            case SDL_SCANCODE_RETURN: case SDL_SCANCODE_SPACE:
                selectCharacter();
                break;
            case SDL_SCANCODE_ESCAPE:
                Game::instance().popState();
                break;
            default: break;
        }
    }
    
    // Pass event to the select button
    for (auto& btn : m_buttons) {
        btn.handleEvent(e);
    }
}

void CharacterSelectState::update(float dt) {
    m_timer += dt;
    for (auto& btn : m_buttons) {
        btn.update(dt);
    }
}

void CharacterSelectState::selectCharacter() {
    Game& g = Game::instance();
    bool unlocked = g.saveData().unlockedChars[m_selected] || m_selected == 0;
    if (!unlocked) {
        g.audio().playSFX("warning");
        return;
    }
    g.saveData().activeCharacter = static_cast<CharacterID>(m_selected);
    g.audio().playSFX("click");
    g.popState();
}

void CharacterSelectState::render() {
    Game& g = Game::instance();
    SDL_Renderer* r = g.getRenderer();
    const ThemeData& theme = g.themes().current();

    // Background
    g.ui().drawAnimatedBackground(theme, m_timer * 0.4f);

    // Title
    g.ui().drawTextWithShadow("Choose Your Heroine",
        EC::SCREEN_WIDTH / 2, 20,
        EC::FONT_SIZE_LARGE, {255, 255, 255, 255}, true);

    // Decorative line
    SDL_SetRenderDrawColor(r, theme.accentColor.r, theme.accentColor.g,
                           theme.accentColor.b, 180);
    SDL_FRect sep = { 30.0f, 68.0f, static_cast<float>(EC::SCREEN_WIDTH) - 60.0f, 2.0f };
    SDL_RenderFillRect(r, &sep);

    g.ui().drawVignette(0.4f, { 0, 0, 0, 255 });

    // Character portrait area
    float portraitCX = static_cast<float>(EC::SCREEN_WIDTH) / 2.0f;
    float portraitCY = static_cast<float>(EC::SCREEN_HEIGHT) * 0.40f;
    float portraitR  = 75.0f;

    // Portrait glow / aura
    const CharInfo& info = CHAR_INFO[static_cast<size_t>(m_selected)];
    float bob = sinf(m_timer * 2.5f) * 8.0f;
    
    // Dynamic nested aura
    for (int i = 3; i > 0; i--) {
        float pulse = sinf(m_timer * 4.0f + i) * 0.2f + 0.6f;
        float glowA = 40.0f * (4.0f - i) * pulse;
        g.ui().drawCircleFilled(portraitCX, portraitCY + bob, portraitR + (float)i * 12.0f,
            { info.color.r, info.color.g, info.color.b, (Uint8)glowA });
    }

    // Main portrait container (Premium Glass)
    Rect portraitBox = { portraitCX - portraitR, portraitCY - portraitR + bob, portraitR * 2.0f, portraitR * 2.0f };
    g.ui().drawGlowPanel(portraitBox, { 25, 15, 45, 210 }, { info.color.r, info.color.g, info.color.b, 50 }, 15.0f);
    g.ui().drawRoundedRectOutline(portraitBox, 16.0f, 2.0f, { info.color.r, info.color.g, info.color.b, 150 });

    // Lock indicator layer
    bool unlocked = g.saveData().unlockedChars[m_selected] || m_selected == 0;
    if (!unlocked) {
        g.ui().drawRoundedRectFilled(portraitBox, 16.0f, { 10, 5, 20, 150 });
        g.ui().drawGlowText("LOCKED", static_cast<int>(portraitCX),
                        static_cast<int>(portraitCY - 20.0f + bob),
                        EC::FONT_SIZE_LARGE, {200, 200, 220, 200}, true);
    }
    else {
        // High quality premium procedural character art
        g.ui().drawPremiumCharacter(portraitCX, portraitCY - 15.0f, 1.2f, 
                                     static_cast<CharacterID>(m_selected), bob);
    }

    // Character name with high-contrast glow
    g.ui().drawGlowText(info.name,
        EC::SCREEN_WIDTH / 2, static_cast<int>(portraitCY + portraitR + 25.0f),
        EC::FONT_SIZE_LARGE, {255, 255, 255, 255}, true);

    // Character ability
    g.ui().drawText("~ " + std::string(info.ability) + " ~",
        EC::SCREEN_WIDTH / 2, static_cast<int>(portraitCY + portraitR + 58.0f),
        EC::FONT_SIZE_MEDIUM, theme.accentColor, true);

    // Unlock condition
    g.ui().drawText(unlocked ? "UNLOCKED" : "Unlock: " + std::string(info.unlockHint),
        EC::SCREEN_WIDTH / 2, static_cast<int>(portraitCY + portraitR + 92.0f),
        EC::FONT_SIZE_SMALL,
        unlocked ? Color{100, 255, 150, 200} : Color{200, 180, 100, 200},
        true);

    // Navigation arrows
    g.ui().drawText("<", 20, static_cast<int>(portraitCY), EC::FONT_SIZE_LARGE, {255, 255, 255, 200});
    g.ui().drawText(">", EC::SCREEN_WIDTH - 40, static_cast<int>(portraitCY), EC::FONT_SIZE_LARGE, {255, 255, 255, 200});

    // Character dots
    float dotY = static_cast<float>(EC::SCREEN_HEIGHT) * 0.78f;
    float dotGap = 16.0f;
    float startX = portraitCX - (static_cast<int>(CharacterID::COUNT) * dotGap) / 2.0f;
    for (int i = 0; i < static_cast<int>(CharacterID::COUNT); i++) {
        bool isActive = (i == m_selected);
        SDL_SetRenderDrawColor(r, theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, isActive ? 255 : 80);
        float dotSize = isActive ? 10.0f : 6.0f;
        SDL_FRect dotRect = { startX + i * dotGap + (10.0f - dotSize) / 2.0f, dotY - dotSize / 2.0f, dotSize, dotSize };
        SDL_RenderFillRect(r, &dotRect);
    }

    // Select button
    if (unlocked) {
        for (auto& btn : m_buttons) {
            btn.render(r);
        }
    }

    // Back hint
    g.ui().drawText("ESC to go back",
        EC::SCREEN_WIDTH / 2, EC::SCREEN_HEIGHT - 26,
        EC::FONT_SIZE_SMALL, {200, 200, 200, 130}, true);
}

} // namespace EC
