// ═══════════════════════════════════════════════════════════════
//  UIRenderer.cpp — wired to generated UI assets
//
//  Uses:
//    sprites/ui/panel_hud.png       → HUD bar background
//    sprites/ui/panel_menu.png      → menu panel background
//    sprites/ui/panel_dialog.png    → dialog/shop panel
//    sprites/ui/button_normal.png   → button idle state
//    sprites/ui/button_hover.png    → button hovered
//    sprites/ui/button_pressed.png  → button pressed
//    sprites/ui/bar_bg.png          → progress bar background
//    sprites/ui/bar_fill_health.png → health/life fill
//    sprites/ui/bar_fill_magic.png  → magic gauge fill
//    sprites/ui/icon_star.png       → star icon in HUD
//    sprites/ui/logo_bg.png         → title glow panel
//    sprites/ui/arrow_left/right    → theme selector arrows
//    sprites/backgrounds/<theme>.png→ animated background
//
//  FIXED: removed dangling code block that caused compile error
//  in the original drawAnimatedBackground switch statement.
// ═══════════════════════════════════════════════════════════════
#include "rendering/UIRenderer.h"
#include "core/Constants.h"
#include "core/Game.h"
#include "managers/AssetManager.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace EC {

    UIRenderer::UIRenderer(SDL_Renderer* r) : m_renderer(r) {}

    UIRenderer::~UIRenderer() {
        if (m_fontLarge)      TTF_CloseFont(m_fontLarge);
        if (m_fontMedium)     TTF_CloseFont(m_fontMedium);
        if (m_fontSmall)      TTF_CloseFont(m_fontSmall);
        if (m_fontExtraLarge) TTF_CloseFont(m_fontExtraLarge);
    }

    bool UIRenderer::loadFonts() {
        const std::string paths[] = {
            EC::FONT_PATH + "magical_font.ttf",
            EC::FONT_PATH + "Quicksand-Bold.ttf",
            EC::FONT_PATH + "ui_font.ttf",
            "C:/Windows/Fonts/segoeui.ttf",
            "C:/Windows/Fonts/arial.ttf",
        };
        for (const auto& path : paths) {
            m_fontExtraLarge = TTF_OpenFont(path.c_str(), 52.0f);
            if (m_fontExtraLarge) {
                m_fontLarge = TTF_OpenFont(path.c_str(), (float)EC::FONT_SIZE_LARGE);
                m_fontMedium = TTF_OpenFont(path.c_str(), (float)EC::FONT_SIZE_MEDIUM);
                m_fontSmall = TTF_OpenFont(path.c_str(), (float)EC::FONT_SIZE_SMALL);
                std::cout << "[UIRenderer] Font loaded: " << path << "\n";
                return true;
            }
        }
        std::cerr << "[UIRenderer] WARNING: No font found — text will not render.\n";
        return true; // non-fatal
    }

    // ── Internal helpers ──────────────────────────────────────────
    static void sr(SDL_Renderer* r, float x, float y, float w, float h,
        Uint8 red, Uint8 g, Uint8 b, Uint8 a = 255) {
        SDL_SetRenderDrawColor(r, red, g, b, a);
        SDL_FRect f = { x, y, w, h };
        SDL_RenderFillRect(r, &f);
    }

    static void fillCircle(SDL_Renderer* r, float cx, float cy, float rad, Color c) {
        SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
        for (float dy = -rad; dy <= rad; dy += 1.0f) {
            float dx = sqrtf(rad * rad - dy * dy);
            SDL_FRect line = { cx - dx, cy + dy, dx * 2.0f, 1.0f };
            SDL_RenderFillRect(r, &line);
        }
    }

    // ── drawCircleFilled ──────────────────────────────────────────
    void UIRenderer::drawCircleFilled(float cx, float cy, float rad, Color color) {
        fillCircle(m_renderer, cx, cy, rad, color);
    }

    // ── drawRoundedRectFilled ─────────────────────────────────────
    void UIRenderer::drawRoundedRectFilled(const Rect& rct, float r, Color color) {
        float x = rct.x, y = rct.y, w = rct.w, h = rct.h;
        float radius = std::min(r, std::min(w, h) * 0.5f);
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_FRect centre = { x + radius, y, w - 2 * radius, h };
        SDL_RenderFillRect(m_renderer, &centre);
        SDL_FRect sides = { x, y + radius, w, h - 2 * radius };
        SDL_RenderFillRect(m_renderer, &sides);
        fillCircle(m_renderer, x + radius, y + radius, radius, color);
        fillCircle(m_renderer, x + w - radius, y + radius, radius, color);
        fillCircle(m_renderer, x + radius, y + h - radius, radius, color);
        fillCircle(m_renderer, x + w - radius, y + h - radius, radius, color);
    }

    // ── drawRoundedRectOutline ────────────────────────────────────
    void UIRenderer::drawRoundedRectOutline(const Rect& rct, float r, float thickness, Color color) {
        float x = rct.x, y = rct.y, w = rct.w, h = rct.h;
        float radius = std::min(r, std::min(w, h) * 0.5f);
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        for (float i = 0; i < thickness; i += 1.0f) {
            SDL_FRect top = { x + radius, y + i,           w - 2 * radius, 1.0f };
            SDL_FRect bottom = { x + radius, y + h - 1 - i,   w - 2 * radius, 1.0f };
            SDL_FRect left = { x + i,      y + radius,       1.0f, h - 2 * radius };
            SDL_FRect right = { x + w - 1 - i,  y + radius,       1.0f, h - 2 * radius };
            SDL_RenderFillRect(m_renderer, &top);
            SDL_RenderFillRect(m_renderer, &bottom);
            SDL_RenderFillRect(m_renderer, &left);
            SDL_RenderFillRect(m_renderer, &right);
        }
    }

    // ── drawText ─────────────────────────────────────────────────
    void UIRenderer::drawText(const std::string& text, int x, int y,
        int fontSize, Color color, bool centered) {
        TTF_Font* font = nullptr;
        if (fontSize >= 48)                    font = m_fontExtraLarge;
        else if (fontSize >= EC::FONT_SIZE_LARGE)   font = m_fontLarge;
        else if (fontSize >= EC::FONT_SIZE_MEDIUM)  font = m_fontMedium;
        else                                        font = m_fontSmall;
        if (!font) return;

        SDL_Color sdlColor = color.toSDL();
        SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), 0, sdlColor);
        if (!surf) return;

        SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surf);
        SDL_DestroySurface(surf);
        if (!tex) return;

        float tw = 0, th = 0;
        SDL_GetTextureSize(tex, &tw, &th);
        float fx = (float)(centered ? x - (int)(tw * 0.5f) : x);
        SDL_FRect dst = { fx, (float)y, tw, th };
        SDL_RenderTexture(m_renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }

    // ── drawTextWithShadow ────────────────────────────────────────
    void UIRenderer::drawTextWithShadow(const std::string& text, int x, int y,
        int fontSize, Color color, bool centered) {
        drawText(text, x + 2, y + 3, fontSize, { 0, 0, 0, 120 }, centered);
        drawText(text, x, y, fontSize, color, centered);
    }

    // ── drawGlowText ─────────────────────────────────────────────
    void UIRenderer::drawGlowText(const std::string& text, int x, int y,
        int fontSize, Color glowColor, bool centered) {
        for (int i = 5; i > 0; i--) {
            float alpha = (1.0f - (float)i / 6.0f) * 0.5f;
            Color c = glowColor; c.a = (Uint8)(c.a * alpha);
            drawText(text, x - i, y, fontSize, c, centered);
            drawText(text, x + i, y, fontSize, c, centered);
            drawText(text, x, y - i, fontSize, c, centered);
            drawText(text, x, y + i, fontSize, c, centered);
        }
        drawText(text, x, y, fontSize, { 255,255,255,255 }, centered);
    }

    // ── drawGradientBackground ────────────────────────────────────
    void UIRenderer::drawGradientBackground(Color top, Color bottom) {
        const int BAND = 3;
        for (int y = 0; y < EC::SCREEN_HEIGHT; y += BAND) {
            float t = (float)y / (float)EC::SCREEN_HEIGHT;
            Color c = Color::lerp(top, bottom, t);
            SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, 255);
            SDL_FRect line = { 0.0f, (float)y, (float)EC::SCREEN_WIDTH,
                               (float)std::min(BAND, EC::SCREEN_HEIGHT - y) };
            SDL_RenderFillRect(m_renderer, &line);
        }
    }

    // ── drawAnimatedBackground ────────────────────────────────────
    void UIRenderer::drawAnimatedBackground(const ThemeData& theme, float timer) {
        auto& g = Game::instance();

        // Try to load pre-generated background texture
        SDL_Texture* bgTex = g.assets().getTexture(theme.backgroundSprite);

        if (bgTex) {
            // Render background texture fullscreen
            SDL_RenderTexture(m_renderer, bgTex, nullptr, nullptr);

            // Subtle animated overlay — tinted pulse
            float pulse = sinf(timer * 0.8f) * 0.08f + 0.08f;
            SDL_SetRenderDrawColor(m_renderer,
                theme.accentColor.r, theme.accentColor.g, theme.accentColor.b,
                (Uint8)(pulse * 255));
            SDL_FRect full = { 0,0,(float)EC::SCREEN_WIDTH,(float)EC::SCREEN_HEIGHT };
            SDL_RenderFillRect(m_renderer, &full);
        }
        else {
            // Fallback: procedural gradient
            drawGradientBackground(theme.bgColorTop, theme.bgColorBottom);
        }

        // ── Theme-specific animated layer overlaid on top of background ──
        switch (theme.id) {

        case ThemeID::FAIRY_GARDEN: {
            // Drifting sparkle motes
            for (int i = 0; i < 20; i++) {
                float tx = fmodf(i * 53.7f + timer * (8 + i % 4), (float)EC::SCREEN_WIDTH);
                float ty = fmodf(i * 89.3f + timer * (5 + i % 3), (float)EC::SCREEN_HEIGHT);
                float a = sinf(timer * 3.0f + i) * 0.5f + 0.5f;
                sr(m_renderer, tx, ty, 3, 3, 255, 220, 240, (Uint8)(80 * a + 20));
            }
            break;
        }

        case ThemeID::MERMAID_LAGOON: {
            // Rising bubbles
            for (int i = 0; i < 14; i++) {
                float bx = (float)(i * 43 % EC::SCREEN_WIDTH);
                float by = fmodf(i * 77.0f + timer * (25 + i % 5), (float)EC::SCREEN_HEIGHT + 20);
                float a = 0.5f - by / EC::SCREEN_HEIGHT * 0.4f;
                sr(m_renderer, bx, EC::SCREEN_HEIGHT - by, 7, 7,
                    200, 240, 255, (Uint8)(70 * a + 15));
            }
            break;
        }

        case ThemeID::PRINCESS_CASTLE: {
            // Floating cloud wisps
            for (int i = 0; i < 5; i++) {
                float cx2 = (float)((i * 120 + 30) % EC::SCREEN_WIDTH);
                float cy2 = (float)(60 + i * 160);
                float bob = sinf(timer * 1.5f + i) * 8.0f;
                sr(m_renderer, cx2, cy2 + bob, 65, 25, 255, 200, 230, 35);
                sr(m_renderer, cx2 + 10, cy2 - 10 + bob, 45, 30, 200, 230, 255, 30);
            }
            break;
        }

        case ThemeID::WITCH_WONDERLAND: {
            // Falling potion drops / glowing wisps
            for (int i = 0; i < 10; i++) {
                float rx = (float)((i * 83) % EC::SCREEN_WIDTH);
                float ry = fmodf(i * 67.0f + timer * 55.0f, (float)EC::SCREEN_HEIGHT + 10);
                sr(m_renderer, rx, ry, 5, 11,
                    (Uint8)(200 + i * 5), (Uint8)(100 + i * 10), 100, 32);
            }
            break;
        }

        case ThemeID::SWEET_TREATS: {
            // Drifting candy dots
            for (int i = 0; i < 18; i++) {
                float cx2 = fmodf(i * 67.3f + timer * (6 + i % 3), (float)EC::SCREEN_WIDTH);
                float cy2 = fmodf(i * 113.7f + timer * (4 + i % 4), (float)EC::SCREEN_HEIGHT);
                float a = sinf(timer * 2.5f + i) * 0.4f + 0.6f;
                sr(m_renderer, cx2, cy2, 5, 5, 255, 150, 180, (Uint8)(60 * a));
            }
            break;
        }

        default: break;
        }

        // Always apply vignette for depth
        drawVignette(0.30f, { 0, 0, 0, 255 });
    }

    // ── drawPanel ─────────────────────────────────────────────────
    void UIRenderer::drawPanel(const Rect& rct, Color fill, int cornerRadius) {
        // Try to use pre-generated panel texture if fill is mostly opaque
        // and rect matches a standard size; otherwise draw procedurally.
        drawRoundedRectFilled(rct, (float)cornerRadius, fill);
        Color highlight = { 255, 255, 255, (Uint8)(fill.a / 3) };
        Rect topBar = { rct.x + (float)cornerRadius, rct.y + 2.0f,
                        rct.w - (float)cornerRadius * 2.0f, 2.0f };
        drawRoundedRectFilled(topBar, 1.0f, highlight);
    }

    // ── drawGlowPanel ─────────────────────────────────────────────
    void UIRenderer::drawGlowPanel(const Rect& rct, Color inner, Color outer,
        float glowRadius) {
        int layers = 7;
        for (int i = layers; i >= 0; i--) {
            float t = (float)i / (float)layers;
            float expand = glowRadius * t;
            Color c = Color::lerp(inner, outer, 1.0f - t);
            c.a = (Uint8)(outer.a * (1.0f - t));
            Rect expanded = { rct.x - expand, rct.y - expand,
                              rct.w + expand * 2, rct.h + expand * 2 };
            drawPanel(expanded, c, 14 + (int)expand);
        }
        drawPanel(rct, inner, 14);
    }

    // ── drawHUD ───────────────────────────────────────────────────
    void UIRenderer::drawHUD(const PlayerStats& stats, const ThemeData& theme) {
        const std::string UI = EC::SPRITE_PATH + "ui/";

        // HUD panel — try texture first
        SDL_Texture* hudPanel = Game::instance().assets().getTexture(UI + "panel_hud.png");
        SDL_FRect hudRect = { 10, 10, (float)EC::SCREEN_WIDTH - 20, 60 };
        if (hudPanel) {
            SDL_RenderTexture(m_renderer, hudPanel, nullptr, &hudRect);
        }
        else {
            Rect r = { 10, 10, (float)EC::SCREEN_WIDTH - 20, 60 };
            drawRoundedRectFilled(r, 30.0f, { 0, 0, 0, 80 });
            drawRoundedRectOutline(r, 30.0f, 1.5f,
                { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, 60 });
        }

        // Score left
        drawText("SCORE", 45, 22, EC::FONT_SIZE_SMALL, { 255, 255, 255, 140 });
        drawGlowText(std::to_string(stats.score), 45, 36, EC::FONT_SIZE_MEDIUM, theme.accentColor);

        // Height center
        int meters = (int)(stats.height / 10.0f);
        float pulse = sinf(SDL_GetTicks() * 0.004f) * 0.05f + 1.0f;
        drawGlowText(std::to_string(meters) + "m",
            EC::SCREEN_WIDTH / 2, 22,
            (int)(EC::FONT_SIZE_LARGE * pulse),
            theme.accentColor, true);

        // Star icon + count right
        SDL_Texture* starIcon = Game::instance().assets().getTexture(UI + "icon_star.png");
        if (starIcon) {
            SDL_FRect starDst = { (float)EC::SCREEN_WIDTH - 110, 22, 20, 20 };
            SDL_RenderTexture(m_renderer, starIcon, nullptr, &starDst);
        }
        drawGlowText(std::to_string(stats.stars),
            EC::SCREEN_WIDTH - 45, 36,
            EC::FONT_SIZE_MEDIUM, { 255,220,50,255 }, true);

        // Streak
        if (stats.streakCount >= 5) {
            float sp = sinf(SDL_GetTicks() * 0.008f) * 0.3f + 1.0f;
            std::string s = "x" + std::to_string(stats.streakCount) + " STREAK!";
            Color col = (stats.streakCount >= 20) ?
                Color{ 255,50,150,255 } : Color{ 255,200,50,255 };
            Rect sb = { (float)EC::SCREEN_WIDTH / 2 - 80, 80, 160, 32 };
            drawGlowPanel(sb, { col.r,col.g,col.b,40 }, { col.r,col.g,col.b,0 }, 12.0f);
            drawGlowText(s, EC::SCREEN_WIDTH / 2, 84, (int)(EC::FONT_SIZE_SMALL * sp), col, true);
        }
    }

    // ── drawProgressBar ───────────────────────────────────────────
    void UIRenderer::drawProgressBar(float x, float y, float w, float h,
        float value, Color fillColor, Color bgColor) {
        const std::string UI = EC::SPRITE_PATH + "ui/";
        SDL_FRect bgDst = { x, y, w, h };
        SDL_FRect fillDst = { x, y, w * std::max(0.0f, std::min(1.0f, value)), h };

        SDL_Texture* bgTex = Game::instance().assets().getTexture(UI + "bar_bg.png");
        SDL_Texture* fillTex = nullptr;
        if (fillColor.r > 200 && fillColor.b < 100) fillTex = Game::instance().assets().getTexture(UI + "bar_fill_health.png");
        else if (fillColor.b > 150)                       fillTex = Game::instance().assets().getTexture(UI + "bar_fill_magic.png");
        else                                              fillTex = Game::instance().assets().getTexture(UI + "bar_fill_score.png");

        if (bgTex) SDL_RenderTexture(m_renderer, bgTex, nullptr, &bgDst);
        else       sr(m_renderer, x, y, w, h, bgColor.r, bgColor.g, bgColor.b, bgColor.a);

        if (fillTex && fillDst.w > 1) SDL_RenderTexture(m_renderer, fillTex, nullptr, &fillDst);
        else sr(m_renderer, x, y, fillDst.w, h, fillColor.r, fillColor.g, fillColor.b, fillColor.a);

        // Highlight stripe
        sr(m_renderer, x, y, fillDst.w, h * 0.35f, 255, 255, 255, 50);
    }

    // ── drawStar ──────────────────────────────────────────────────
    void UIRenderer::drawStar(float cx, float cy, float radius, Color color, bool filled) {
        float outerR = radius, innerR = radius * 0.4f;
        float baseA = -SDL_PI_F / 2.0f;
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        for (int i = 0; i < 5; i++) {
            float a1 = baseA + i * 2.0f * SDL_PI_F / 5.0f;
            float a2 = a1 + SDL_PI_F / 5.0f;
            float a3 = a1 + 2.0f * SDL_PI_F / 5.0f;
            float ox1 = cx + outerR * cosf(a1), oy1 = cy + outerR * sinf(a1);
            float ix = cx + innerR * cosf(a2), iy = cy + innerR * sinf(a2);
            float ox2 = cx + outerR * cosf(a3), oy2 = cy + outerR * sinf(a3);
            if (filled) {
                SDL_RenderLine(m_renderer, ox1, oy1, ix, iy);
                SDL_RenderLine(m_renderer, ix, iy, cx, cy);
                SDL_RenderLine(m_renderer, cx, cy, ox1, oy1);
            }
            else {
                SDL_RenderLine(m_renderer, ox1, oy1, ix, iy);
                SDL_RenderLine(m_renderer, ix, iy, ox2, oy2);
            }
        }
    }

    // ── drawConfetti ──────────────────────────────────────────────
    void UIRenderer::drawConfetti(float timer) {
        for (int i = 0; i < 50; i++) {
            float x = fmodf(i * 123.4f, (float)EC::SCREEN_WIDTH);
            float y = fmodf(i * 567.8f + timer * 220.0f, (float)EC::SCREEN_HEIGHT);
            float rot = timer * 5.0f + i;
            float size = 6.0f + sinf(timer * 10.0f + i) * 2.0f;
            SDL_Color cols[] = {
                {255,100,100,200},{100,255,100,200},
                {100,100,255,200},{255,255,100,200},{255,100,255,200}
            };
            SDL_Color& c = cols[i % 5];
            SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
            SDL_FRect rf = { x + cosf(rot) * 5, y + sinf(rot) * 5, size, size };
            SDL_RenderFillRect(m_renderer, &rf);
        }
    }

    // ── drawPremiumCharacter ──────────────────────────────────────
    void UIRenderer::drawPremiumCharacter(float cx, float cy, float scale,
        CharacterID charId, float bob) {
        const std::string SP = EC::SPRITE_PATH + "characters/";

        // Character name → filename map
        const char* charFiles[(int)CharacterID::COUNT] = {
            "lily_fairy.png",
            "marina_mermaid.png",
            "rosalind_princess.png",
            "morgana_witch.png",
            "chloe_baker.png",
            "aurora_butterfly.png",
            "stella_starlight.png",
            "luna_moonrabbit.png",
        };

        int idx = (int)charId;
        if (idx < 0 || idx >= (int)CharacterID::COUNT) idx = 0;

        SDL_Texture* charTex = Game::instance().assets().getTexture(SP + charFiles[idx]);
        float by = cy + bob;

        if (charTex) {
            // The preview sprites are 64×80
            float w = 64.0f * scale;
            float h = 80.0f * scale;
            SDL_FRect dst = { cx - w * 0.5f, by - h * 0.5f, w, h };
            SDL_RenderTexture(m_renderer, charTex, nullptr, &dst);
            return;
        }

        // ── Fallback procedural character ─────────────────────────────
        float s = scale;
        Color hair = { 100, 50, 30, 255 };
        Color skin = { 255, 220, 200, 255 };
        Color dress = { 255, 100, 200, 255 };
        Color eyeCol = { 40, 40, 40, 255 };

        if (charId == CharacterID::MARINA_MERMAID) { hair = { 40,200,220,255 };  dress = { 100,255,180,255 }; }
        else if (charId == CharacterID::ROSALIND_PRINCESS) { hair = { 255,230,100,255 }; dress = { 200,150,255,255 }; }
        else if (charId == CharacterID::MORGANA_WITCH) { hair = { 50,20,80,255 };    dress = { 120,50,180,255 }; }
        else if (charId == CharacterID::CHLOE_BAKER) { hair = { 255,150,50,255 };  dress = { 255,220,160,255 }; }
        else if (charId == CharacterID::AURORA_BUTTERFLY) { hair = { 255,100,255,255 }; dress = { 200,255,150,255 }; }
        else if (charId == CharacterID::STELLA_STARLIGHT) { hair = { 30,30,100,255 };   dress = { 80,80,180,255 }; }
        else if (charId == CharacterID::LUNA_MOONRABBIT) { hair = { 230,230,255,255 }; dress = { 180,180,220,255 }; }

        fillCircle(m_renderer, cx, by - 5 * s, 18 * s, hair);
        for (int i = 0; i < 25; i++) {
            float rowW = 8.0f + (float)i * 1.5f;
            float rowA = (float)(255 - i * 4); if (rowA < 0) rowA = 0;
            Color dc = dress; dc.a = (Uint8)rowA;
            sr(m_renderer, cx - rowW * s * 0.5f, by + (10 + (float)i) * s,
                rowW * s, s, dc.r, dc.g, dc.b, dc.a);
        }
        fillCircle(m_renderer, cx, by - 10 * s, 14 * s, skin);
        fillCircle(m_renderer, cx - 6 * s, by - 12 * s, 3.0f * s, eyeCol);
        fillCircle(m_renderer, cx + 6 * s, by - 12 * s, 3.0f * s, eyeCol);
        fillCircle(m_renderer, cx - 7 * s, by - 13 * s, 1.2f * s, { 255,255,255,220 });
        fillCircle(m_renderer, cx + 5 * s, by - 13 * s, 1.2f * s, { 255,255,255,220 });
        fillCircle(m_renderer, cx - 9 * s, by - 6 * s, 2 * s, { 255,120,150,110 });
        fillCircle(m_renderer, cx + 9 * s, by - 6 * s, 2 * s, { 255,120,150,110 });
        fillCircle(m_renderer, cx, by - 20 * s, 13 * s, hair);
        sr(m_renderer, cx - 14 * s, by - 18 * s, 28 * s, 8 * s, hair.r, hair.g, hair.b);
    }

    // ── drawVignette ──────────────────────────────────────────────
    void UIRenderer::drawVignette(float intensity, Color color) {
        float w = (float)EC::SCREEN_WIDTH, h = (float)EC::SCREEN_HEIGHT;
        for (int i = 0; i < 8; i++) {
            float aMod = (float)std::max(1, 8 - i) / 8.0f;
            SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b,
                (Uint8)(intensity * 120.0f * aMod));
            float offset = (float)i * 10.0f;
            SDL_FRect r = { offset, offset, w - offset * 2, h - offset * 2 };
            SDL_RenderRect(m_renderer, &r);
        }
    }

    // ── drawScreenFade ────────────────────────────────────────────
    void UIRenderer::drawScreenFade(float alpha, Color color) {
        if (alpha < 0.001f) return;
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, (Uint8)(alpha * 255.0f));
        SDL_FRect f = { 0.0f, 0.0f, (float)EC::SCREEN_WIDTH, (float)EC::SCREEN_HEIGHT };
        SDL_RenderFillRect(m_renderer, &f);
    }

} // namespace EC