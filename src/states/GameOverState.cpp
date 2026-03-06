// ═══════════════════════════════════════════════════════════════
//  GameOverState.cpp — Magical game over screen with stats
// ═══════════════════════════════════════════════════════════════
#include "states/GameOverState.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "states/MenuState.h"
#include "states/PlayingState.h"
#include "rendering/UIRenderer.h"
#include "managers/AudioManager.h"
#include "managers/ThemeManager.h"
#include "rendering/ParticleSystem.h"
#include "ui/Button.h"
#include <cmath>
#include <string>
#include <memory>

namespace EC {

GameOverState::GameOverState(const PlayerStats& stats) : m_stats(stats) {}

void GameOverState::onEnter() {
    Game& g = Game::instance();
    auto& save = g.saveData();

    m_newHighScore = m_stats.score > save.highScore;
    if (m_newHighScore) {
        save.highScore     = m_stats.score;
        save.highestHeight = m_stats.height;
        g.particles().spawnHighScore({ static_cast<float>(EC::SCREEN_WIDTH) / 2.0f, static_cast<float>(EC::SCREEN_HEIGHT) / 3.0f });
    }

    float cx = static_cast<float>(EC::SCREEN_WIDTH) / 2.0f;
    float by = static_cast<float>(EC::SCREEN_HEIGHT) * 0.65f;
    float bw = 210.0f, bh = 54.0f;

    m_buttons.clear();

    Button retryBtn, menuBtn;
    retryBtn.init(cx - bw / 2.0f, by, bw, bh, "TRY AGAIN",
        []() { Game::instance().changeState(std::make_unique<PlayingState>()); });
    menuBtn.init(cx - bw / 2.0f, by + 68.0f, bw, bh, "EXIT TO MENU",
        []() { Game::instance().changeState(std::make_unique<MenuState>()); });

    m_buttons.push_back(std::move(retryBtn));
    m_buttons.push_back(std::move(menuBtn));

    m_timer = 0.0f;
}

void GameOverState::onExit() { m_buttons.clear(); }

void GameOverState::handleEvent(const SDL_Event& e) {
    for (auto& b : m_buttons) b.handleEvent(e);
}

void GameOverState::update(float dt) {
    m_timer += dt;
    for (auto& b : m_buttons) b.update(dt);
}

void GameOverState::render() {
    Game& g = Game::instance();
    SDL_Renderer* r = g.getRenderer();
    const ThemeData& theme = g.themes().current();

    // Background & Cinematic Atmosphere
    g.ui().drawAnimatedBackground(theme, m_timer * 0.3f);
    g.ui().drawScreenFade(0.6f, { 15, 10, 30, 255 });
    g.ui().drawVignette(0.7f, { 0, 0, 0, 255 });

    // Central panel (Rounded & Glassy)
    Rect panelRect = { 30.0f, static_cast<float>(EC::SCREEN_HEIGHT) * 0.12f,
                       static_cast<float>(EC::SCREEN_WIDTH) - 60.0f, static_cast<float>(EC::SCREEN_HEIGHT) * 0.48f };
    Color panelFill = { 25, 15, 45, 230 };
    Color panelGlow = { theme.accentColor.r, theme.accentColor.g,
                        theme.accentColor.b, 60 };
    g.ui().drawGlowPanel(panelRect, panelFill, panelGlow, 22.0f);
    g.ui().drawRoundedRectOutline(panelRect, 20.0f, 2.0f, { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 100 });

    // Title with multi-layered glow
    float titleY = static_cast<float>(EC::SCREEN_HEIGHT) * 0.17f;
    float titleBob = sinf(m_timer * 2.5f) * 6.0f;
    g.ui().drawGlowText("GAME OVER",
        EC::SCREEN_WIDTH / 2, static_cast<int>(titleY + titleBob),
        EC::FONT_SIZE_LARGE + 12, {255, 100, 150, 255}, true);

    // Dynamic Stats display
    float currentStatsY = titleY + 80.0f;
    float statsGapBy = 42.0f;

    auto drawStatLine = [&](const std::string& label, const std::string& val, Color col) {
        float rowW = panelRect.w - 40;
        Rect rowRect = { panelRect.x + 20, currentStatsY - 8, rowW, 36 };
        g.ui().drawRoundedRectFilled(rowRect, 12.0f, { 255, 255, 255, 15 });
        
        g.ui().drawText(label, (int)(panelRect.x + 35), (int)currentStatsY, EC::FONT_SIZE_SMALL, {200, 200, 255, 180});
        g.ui().drawGlowText(val, (int)(panelRect.right() - 35), (int)currentStatsY, EC::FONT_SIZE_MEDIUM, col, true); 
    };

    int meters = static_cast<int>(m_stats.height / 10.0f);
    drawStatLine("HEIGHT REACHED", std::to_string(meters) + " m", {255,255,255,255});
    currentStatsY += statsGapBy;
    
    drawStatLine("TOTAL SCORE", std::to_string(m_stats.score), {255, 255, 255, 255});
    currentStatsY += statsGapBy;
    
    drawStatLine("STARS COLLECTED", std::to_string(m_stats.stars), {255, 215, 50, 255});
    currentStatsY += statsGapBy;
    
    drawStatLine("BEST STREAK", "x" + std::to_string(m_stats.streakCount), {120, 255, 200, 255});

    // High score banner
    if (m_newHighScore) {
        float bannerY = static_cast<float>(EC::SCREEN_HEIGHT) * 0.54f;
        float pulseRate = sinf(m_timer * 5.0f) * 0.5f + 0.5f;
        Rect bannerRect = { 40.0f, bannerY - 12.0f, static_cast<float>(EC::SCREEN_WIDTH) - 80.0f, 44.0f };
        
        g.ui().drawGlowPanel(bannerRect, { 80, 40, 0, (Uint8)(200 + 55 * pulseRate) }, { 255, 215, 50, 40 }, 8.0f);
        g.ui().drawText("NEW PERSONAL BEST!",
            EC::SCREEN_WIDTH / 2, static_cast<int>(bannerY),
            EC::FONT_SIZE_SMALL, {255, 255, 255, 255}, true);
    }

    // Best record text
    g.ui().drawText("ALL-TIME BEST:  " + std::to_string(g.saveData().highScore),
        EC::SCREEN_WIDTH / 2, static_cast<int>(EC::SCREEN_HEIGHT * 0.62f),
        EC::FONT_SIZE_SMALL, {200, 200, 200, 150}, true);

    // UI Buttons
    for (auto& b : m_buttons) b.render(r);

    // Particles
    g.particles().render(r);
}

} // namespace EC
