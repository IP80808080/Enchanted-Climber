#pragma once
// ═══════════════════════════════════════════════════════════════
//  UIRenderer.h — Premium visual rendering system
//  SDL3 core  → <SDL3/SDL.h>
//  SDL3_ttf   → <SDL3_ttf/SDL_ttf.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "core/Types.h"
#include "core/Constants.h"
#include <string>

namespace EC {

    class UIRenderer {
    public:
        explicit UIRenderer(SDL_Renderer* r);
        ~UIRenderer();

        bool loadFonts();

        // ── Text ────────────────────────────────────────────────────
        void drawText(const std::string& text, int x, int y,
            int fontSize, Color color, bool centered = false);
        void drawTextWithShadow(const std::string& text, int x, int y,
            int fontSize, Color color, bool centered = false);
        void drawGlowText(const std::string& text, int x, int y,
            int fontSize, Color glowColor, bool centered = false);

        // ── Background ──────────────────────────────────────────────
        void drawGradientBackground(Color top, Color bottom);
        void drawAnimatedBackground(const ThemeData& theme, float timer);

        // ── Panels ──────────────────────────────────────────────────
        void drawPanel(const Rect& r, Color fill, int cornerRadius = 12);
        void drawGlowPanel(const Rect& r, Color inner, Color outer,
            float glowRadius = 12.0f);

        // ── HUD ─────────────────────────────────────────────────────
        void drawHUD(const PlayerStats& stats, const ThemeData& theme);

        // ── Widgets ─────────────────────────────────────────────────
        void drawProgressBar(float x, float y, float w, float h,
            float value, Color fillColor, Color bgColor);
        void drawStar(float cx, float cy, float radius,
            Color color, bool filled = true);
        void drawConfetti(float timer);
        void drawCircleFilled(float cx, float cy, float rad, Color color);
        void drawRoundedRectFilled(const Rect& rct, float r, Color color);
        void drawRoundedRectOutline(const Rect& rct, float r, float thickness, Color color);
        void drawVignette(float intensity, Color color = {0,0,0,255});
        void drawScreenFade(float alpha, Color color = {0,0,0,255});

        // ── Character preview ───────────────────────────────────────
        void drawPremiumCharacter(float cx, float cy, float scale,
            CharacterID charId, float bob);

    private:
        SDL_Renderer* m_renderer = nullptr;
        TTF_Font* m_fontExtraLarge = nullptr;
        TTF_Font* m_fontLarge = nullptr;
        TTF_Font* m_fontMedium = nullptr;
        TTF_Font* m_fontSmall = nullptr;
    };

} // namespace EC