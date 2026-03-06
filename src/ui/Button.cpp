// ═══════════════════════════════════════════════════════════════
//  Button.cpp — Premium magical button rendering
//  SDL3: SDL_EVENT_MOUSE_*, SDL_RenderFillRect, SDL_RenderRect
//
//  IMPORTANT: Game.h only forward-declares managers.
//  We must include each manager's full header here directly.
// ═══════════════════════════════════════════════════════════════
#include "ui/Button.h"
#include "core/Game.h"
#include "core/Constants.h"
// Full headers needed — Game.h only forward-declares these
#include "managers/AudioManager.h"
#include "managers/ThemeManager.h"
#include "rendering/UIRenderer.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <algorithm>

namespace EC {

    void Button::init(float x, float y, float w, float h,
        const std::string& label,
        std::function<void()> onClick) {
        m_rect = { x, y, w, h };
        m_label = label;
        m_onClick = onClick;
        m_scale = 1.0f;
        m_glowAlpha = 0.0f;
        m_pulseTimer = 0.0f;
    }

    // ── handleEvent ────────────────────────────────────────────────
    void Button::handleEvent(const SDL_Event& e) {
        if (e.type == SDL_EVENT_MOUSE_MOTION) {
            float mx = e.motion.x, my = e.motion.y;
            bool was = m_hovered;
            m_hovered = (mx >= m_rect.x && mx <= m_rect.right() &&
                my >= m_rect.y && my <= m_rect.bottom());
            if (m_hovered && !was)
                Game::instance().audio().playSFX("hover");
        }
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && m_hovered) {
            m_pressed = true;
            Game::instance().audio().playSFX("click");
        }
        if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            if (m_pressed && m_hovered && m_onClick) m_onClick();
            m_pressed = false;
        }
    }

    // ── update ─────────────────────────────────────────────────────
    void Button::update(float dt) {
        float targetScale = m_hovered ? EC::BUTTON_HOVER_SCALE : 1.0f;
        if (m_pressed) targetScale = 0.96f;
        m_scale += (targetScale - m_scale) * 12.0f * dt;
        m_glowAlpha += ((m_hovered ? 1.0f : 0.0f) - m_glowAlpha) * 10.0f * dt;
        m_pulseTimer += dt;
    }

    // ── render ─────────────────────────────────────────────────────
    void Button::render(SDL_Renderer* r) {
        auto& g = Game::instance();
        const ThemeData& theme = g.themes().current();

        float cx = m_rect.centerX();
        float cy = m_rect.centerY();
        float sw = m_rect.w * m_scale;
        float sh = m_rect.h * m_scale;
        Rect scaledRect = { cx - sw * 0.5f, cy - sh * 0.5f, sw, sh };

        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

        // 1. Premium Glow Aura
        if (m_glowAlpha > 0.01f) {
            Color glowColor = theme.accentColor;
            glowColor.a = (Uint8)(50 * m_glowAlpha);
            g.ui().drawGlowPanel(scaledRect, glowColor, { glowColor.r, glowColor.g, glowColor.b, 0 }, 15.0f * m_glowAlpha);
        }

        // 2. Drop Shadow
        Rect shadowRect = { scaledRect.x + 4, scaledRect.y + 6, sw, sh };
        g.ui().drawRoundedRectFilled(shadowRect, 16.0f, { 0, 0, 0, 90 });

        // 3. Button Body - Deep Frosted Glass
        Uint8 bodyA = m_pressed ? 220 : 190;
        g.ui().drawRoundedRectFilled(scaledRect, 16.0f, { 25, 15, 45, bodyA });
        g.ui().drawRoundedRectFilled(scaledRect, 16.0f, { theme.accentColor.r, theme.accentColor.g, theme.accentColor.b, (Uint8)(m_hovered ? 40 : 20) });

        // 4. Glass Highlight (Top half)
        Rect highlightRect = { scaledRect.x + 3, scaledRect.y + 3, sw - 6, sh * 0.45f };
        g.ui().drawRoundedRectFilled(highlightRect, 12.0f, { 255, 255, 255, (Uint8)(m_hovered ? 50 : 30) });

        // 5. Accent Border
        Color borderColor = theme.accentColor;
        borderColor.a = (Uint8)(m_hovered ? 200 : 100);
        g.ui().drawRoundedRectOutline(scaledRect, 16.0f, 2.0f, borderColor);

        // 6. Label with dynamic styling
        Color labelColor = { 255, 255, 255, 255 };
        float labelOffset = m_pressed ? 2.0f : 0.0f;
        float labelY = cy - (float)EC::FONT_SIZE_MEDIUM * 0.5f + labelOffset;
        
        // Label shadow for contrast
        g.ui().drawText(m_label, (int)cx + 1, (int)labelY + 1, EC::FONT_SIZE_MEDIUM, { 0, 0, 0, 150 }, true);
        
        if (m_hovered) {
            g.ui().drawGlowText(m_label, (int)cx, (int)labelY, EC::FONT_SIZE_MEDIUM, theme.accentColor, true);
        } else {
            g.ui().drawText(m_label, (int)cx, (int)labelY, EC::FONT_SIZE_MEDIUM, labelColor, true);
        }
    }

} // namespace EC