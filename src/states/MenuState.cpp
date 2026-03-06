// ═══════════════════════════════════════════════════════════════
//  MenuState.cpp — World-class main menu
//  Cinematic layered background, floating character preview,
//  crystal-glass buttons, animated logo, particle storm
// ═══════════════════════════════════════════════════════════════
#include "states/MenuState.h"
#include "core/Game.h"
#include "core/Constants.h"
#include "states/PlayingState.h"
#include "states/ShopState.h"
#include "states/CharacterSelectState.h"
#include "managers/AudioManager.h"
#include "managers/ThemeManager.h"
#include "rendering/ParticleSystem.h"
#include "rendering/UIRenderer.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <memory>

namespace EC {


    void MenuState::onEnter() {
        Game& g = Game::instance();
        m_timer = m_bgScrollY = m_themeScrollTimer = 0.0f;

        g.particles().spawnTransform(
            { EC::SCREEN_WIDTH * 0.5f, EC::SCREEN_HEIGHT * 0.5f },
            g.themes().activeID());
        g.audio().playThemeMusic(g.themes().activeID());

        float cx = EC::SCREEN_WIDTH * 0.5f;
        float by = EC::SCREEN_HEIGHT * 0.565f;
        float bw = 260.0f, bh = 58.0f, gap = 72.0f;

        m_buttons.clear();
        Button b0, b1, b2, b3;
        b0.init(cx - bw / 2, by, bw, bh, "PLAY NOW",
            []() { Game::instance().changeState(std::make_unique<PlayingState>()); });
        b1.init(cx - bw / 2, by + gap, bw, bh, "CHARACTERS",
            []() { Game::instance().pushState(std::make_unique<CharacterSelectState>()); });
        b2.init(cx - bw / 2, by + gap * 2, bw, bh, "MAGIC SHOP",
            []() { Game::instance().pushState(std::make_unique<ShopState>()); });
        b3.init(cx - bw / 2, by + gap * 3, bw, bh, "EXIT",
            []() { Game::instance().requestQuit(); });
        m_buttons.push_back(std::move(b0)); m_buttons.push_back(std::move(b1));
        m_buttons.push_back(std::move(b2)); m_buttons.push_back(std::move(b3));
    }

    void MenuState::onExit() {
        Game::instance().particles().clear();
        m_buttons.clear();
    }

    void MenuState::handleEvent(const SDL_Event& e) {
        if (e.type == SDL_EVENT_KEY_DOWN) {
            if (e.key.scancode == SDL_SCANCODE_LEFT || e.key.scancode == SDL_SCANCODE_A) cycleTheme(-1);
            if (e.key.scancode == SDL_SCANCODE_RIGHT || e.key.scancode == SDL_SCANCODE_D) cycleTheme(1);
        }
        for (auto& b : m_buttons) b.handleEvent(e);
    }

    void MenuState::update(float dt) {
        m_timer += dt; m_themeScrollTimer += dt;
        m_titleBob = sinf(m_timer * 2.2f) * 9.0f;
        m_bgScrollY = fmodf(m_bgScrollY + 38.0f * dt, (float)EC::SCREEN_HEIGHT);
        for (auto& b : m_buttons) b.update(dt);
        static float pTimer = 0.0f;
        pTimer += dt;
        if (pTimer > 0.18f) {
            pTimer = 0.0f;
            Game& g = Game::instance();
            float rx = 10.0f + (rand() % (EC::SCREEN_WIDTH - 20));
            float ry = 10.0f + (rand() % (EC::SCREEN_HEIGHT - 20));
            g.particles().spawnJump({ rx,ry }, g.themes().activeID());
        }
        Game::instance().particles().update(dt);
    }

    void MenuState::cycleTheme(int dir) {
        Game& g = Game::instance();
        int next = ((int)g.themes().activeID() + dir + (int)ThemeID::COUNT) % (int)ThemeID::COUNT;
        g.themes().setTheme((ThemeID)next);
        g.saveData().activeTheme = (ThemeID)next;
        g.audio().playThemeMusic((ThemeID)next);
        g.particles().spawnTransform(
            { EC::SCREEN_WIDTH * 0.5f, EC::SCREEN_HEIGHT * 0.5f }, (ThemeID)next);
    }

    void MenuState::render() {
        Game& g = Game::instance();
        SDL_Renderer* r = g.getRenderer();
        const ThemeData& theme = g.themes().current();
        const float W = EC::SCREEN_WIDTH, H = EC::SCREEN_HEIGHT;

        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

        // ── 1. Animated background ──────────────────────────────────
        g.ui().drawAnimatedBackground(theme, m_timer);

        // ── 2. Diagonal scrolling shimmer bands ─────────────────────
        for (int i = 0; i < 10; i++) {
            float bandY = fmodf(i * 100.0f + m_bgScrollY * 1.8f, H + 120.0f);
            float alpha = sinf(m_timer * 1.2f + i * 0.5f) * 12.0f + 18.0f;
            SDL_SetRenderDrawColor(r,
                theme.accentColor.r, theme.accentColor.g, theme.accentColor.b,
                (Uint8)alpha);
            SDL_RenderLine(r, 0, bandY, W, bandY + 80.0f);
            SDL_RenderLine(r, 0, bandY + 2, W, bandY + 82.0f);
        }

        // ── 3. Floating character preview with deep glow ─────────────
        float charX = W * 0.72f;
        float charY = H * 0.29f + m_titleBob * 0.6f;
        // Character aura — pulsed glow
        for (int i = 5; i >= 1; i--) {
            float ar = (float)i * 20.0f;
            float aa = (sinf(m_timer * 2.5f) * 0.2f + 0.8f) * (20.0f / (float)i);
            g.ui().drawCircleFilled(charX, charY + 15, ar,
                { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, (Uint8)aa });
        }
        g.ui().drawPremiumCharacter(charX, charY, 1.5f,
            g.saveData().activeCharacter,
            sinf(m_timer * 1.8f) * 7.0f);

        // ── 4. Title block ───────────────────────────────────────────
        float titleY = H * 0.11f + m_titleBob;

        // Title glow panel
        Rect gp = { 18, titleY - 28, W - 36, 120 };
        g.ui().drawGlowPanel(gp,
            { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 35 },
            { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 0 },
            24.0f);

        // "Enchanted" — multi-layer glow
        g.ui().drawGlowText("Enchanted",
            (int)(W * 0.5f), (int)titleY,
            EC::FONT_SIZE_LARGE + 12, theme.accentColor, true);
        g.ui().drawGlowText("Climber",
            (int)(W * 0.5f), (int)(titleY + 58),
            EC::FONT_SIZE_LARGE + 12, theme.accentColor, true);

        // Subtitle pulse
        Uint8 sa = (Uint8)(sinf(m_timer * 2.4f) * 60 + 170);
        g.ui().drawText("~ A Magical Adventure ~",
            (int)(W * 0.5f), (int)(titleY + 104),
            EC::FONT_SIZE_SMALL, { 255,255,255,sa }, true);

        // Decorative divider with dots
        float divY = titleY + 126;
        SDL_SetRenderDrawColor(r,
            theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 100);
        SDL_RenderLine(r, 40, divY, (int)(W * 0.38f), divY);
        SDL_RenderLine(r, (int)(W * 0.62f), divY, (int)(W - 40), divY);
        g.ui().drawCircleFilled(W * 0.5f, divY, 4.0f,
            { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 200 });
        g.ui().drawCircleFilled(W * 0.5f - 14, divY, 2.0f,
            { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 120 });
        g.ui().drawCircleFilled(W * 0.5f + 14, divY, 2.0f,
            { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 120 });

        // ── 5. Theme selector pill ───────────────────────────────────
        float themeY = H * 0.515f;
        float pillW = 230.0f, pillH = 34.0f;
        float pillX = W * 0.5f - pillW * 0.5f;
        
        // Pill background (Rounded)
        Rect pillRect = { pillX, themeY, pillW, pillH };
        g.ui().drawRoundedRectFilled(pillRect, 17.0f, 
            { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 40 });
        
        // Border
        SDL_SetRenderDrawColor(r, theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 150);
        SDL_FRect pillBdr = { pillX, themeY, pillW, pillH };
        SDL_RenderRect(r, &pillBdr);
        
        // Arrow indicators
        g.ui().drawText("<", (int)(pillX + 10), (int)(themeY + 8), EC::FONT_SIZE_SMALL, theme.accentColor);
        g.ui().drawText(">", (int)(pillX + pillW - 22), (int)(themeY + 8), EC::FONT_SIZE_SMALL, theme.accentColor);
        g.ui().drawText(theme.displayName,
            (int)(W * 0.5f), (int)(themeY + 8),
            EC::FONT_SIZE_SMALL, { 255,255,255,230 }, true);

        // ── 6. Buttons ───────────────────────────────────────────────
        for (auto& b : m_buttons) b.render(r);

        // ── 7. Particles ─────────────────────────────────────────────
        g.particles().render(r);

        // ── 8. High score badge ──────────────────────────────────────
        const SaveData& save = g.saveData();
        if (save.highScore > 0) {
            float badgeY = H - 65.0f;
            float pulse = sinf(m_timer * 3.0f) * 0.15f + 0.85f;
            Rect badgeRect = { W * 0.5f - 85, badgeY, 170, 30 };
            g.ui().drawRoundedRectFilled(badgeRect, 15.0f, { 60, 40, 0, (Uint8)(200 * pulse) });
            
            g.ui().drawText("✦ BEST: " + std::to_string(save.highScore),
                (int)(W * 0.5f), (int)(badgeY + 8),
                EC::FONT_SIZE_SMALL, { 255,215,50, (Uint8)(230 * pulse) }, true);
        }

        // ── 9. Bottom hint bar (frosted glass) ───────────────────────
        Rect hintRect = { 0, H - 34, W, 34 };
        g.ui().drawRoundedRectFilled(hintRect, 0.0f, { 0, 0, 0, 100 });
        SDL_SetRenderDrawColor(r, theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 80);
        SDL_FRect hintLine = { 0, H - 34, W, 1 };
        SDL_RenderFillRect(r, &hintLine);
        g.ui().drawText(
            "\xE2\x86\x90\xE2\x86\x92  Change Theme  |  WASD / Arrows to Move",
            (int)(W * 0.5f), (int)(H - 22),
            EC::FONT_SIZE_SMALL, { 255,255,255,110 }, true);
    }

} // namespace EC