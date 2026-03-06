// ═══════════════════════════════════════════════════════════════
//  PausedState.cpp — Beautiful pause overlay
// ═══════════════════════════════════════════════════════════════
#include "states/PausedState.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "states/MenuState.h"
#include "rendering/UIRenderer.h"
#include "managers/ThemeManager.h"
#include "rendering/ParticleSystem.h"
#include "ui/Button.h"
#include <cmath>
#include <memory>

namespace EC {

void PausedState::onEnter() {
    float cx = static_cast<float>(EC::SCREEN_WIDTH) / 2.0f;
    float by = static_cast<float>(EC::SCREEN_HEIGHT) * 0.5f;
    float bw = 210.0f, bh = 54.0f;

    m_buttons.clear();

    Button resume, menu;
    resume.init(cx - bw / 2.0f, by, bw, bh, "CONTINUE",
        []() { Game::instance().popState(); });
    menu.init(cx - bw / 2.0f, by + 68.0f, bw, bh, "MAIN MENU",
        []() {
            Game::instance().clearAndPush(std::make_unique<MenuState>());
        });

    m_buttons.push_back(std::move(resume));
    m_buttons.push_back(std::move(menu));
    m_timer = 0.0f;
}

void PausedState::onExit()   { m_buttons.clear(); }
void PausedState::onResume() {}
void PausedState::onPause()  {}

void PausedState::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_EVENT_KEY_DOWN &&
        e.key.scancode == SDL_SCANCODE_ESCAPE) {
        Game::instance().popState();
        return;
    }
    for (auto& b : m_buttons) b.handleEvent(e);
}

void PausedState::update(float dt) {
    m_timer += dt;
    for (auto& b : m_buttons) b.update(dt);
}

void PausedState::render() {
    Game& g = Game::instance();
    SDL_Renderer* r = g.getRenderer();
    const ThemeData& theme = g.themes().current();

    // Dark translucent background with theme vignette
    g.ui().drawAnimatedBackground(theme, m_timer * 0.2f);
    
    SDL_SetRenderDrawColor(r, 0, 0, 0, 150);
    SDL_FRect overlayRect = { 0.0f, 0.0f, static_cast<float>(EC::SCREEN_WIDTH), static_cast<float>(EC::SCREEN_HEIGHT) };
    SDL_RenderFillRect(r, &overlayRect);

    // Accent edge glow
    SDL_SetRenderDrawColor(r, theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 30);
    for (int i = 0; i < 4; i++) {
        float off = (float)i * 4.0f;
        SDL_FRect border = { off, off, (float)EC::SCREEN_WIDTH - off * 2, (float)EC::SCREEN_HEIGHT - off * 2 };
        SDL_RenderRect(r, &border);
    }

    // Central glass panel
    float panelW = static_cast<float>(EC::SCREEN_WIDTH) - 80.0f;
    float panelH = static_cast<float>(EC::SCREEN_HEIGHT) * 0.35f;
    Rect panelRect = { 40.0f, static_cast<float>(EC::SCREEN_HEIGHT) * 0.28f, panelW, panelH };
    
    g.ui().drawGlowPanel(panelRect, { 25, 15, 45, 210 }, 
        { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 80 }, 18.0f);

    // Title with deep glow
    float titleY = panelRect.y + 45.0f;
    float bob = sinf(m_timer * 2.5f) * 6.0f;
    g.ui().drawGlowText("PAUSED",
        EC::SCREEN_WIDTH / 2, static_cast<int>(titleY + bob),
        EC::FONT_SIZE_LARGE + 8, {255, 255, 255, 255}, true);

    // Tip
    g.ui().drawText("Adventure Awaits...",
        EC::SCREEN_WIDTH / 2,
        static_cast<int>(titleY + 70.0f),
        EC::FONT_SIZE_SMALL,
        {200, 200, 255, 180}, true);

    for (auto& b : m_buttons) b.render(r);
    
    // UI Particles
    g.particles().render(r);
}

} // namespace EC
