#pragma once
// ═══════════════════════════════════════════════════════════════
//  Button.h
//  SDL3 core → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "core/Types.h"
#include <string>
#include <functional>

namespace EC {

    class Button {
    public:
        void init(float x, float y, float w, float h,
            const std::string& label,
            std::function<void()> onClick);

        void handleEvent(const SDL_Event& e);
        void update(float dt);
        void render(SDL_Renderer* r);

        bool isHovered() const { return m_hovered; }

    private:
        Rect                  m_rect;
        std::string           m_label;
        std::function<void()> m_onClick;
        bool                  m_hovered = false;
        bool                  m_pressed = false;
        float                 m_scale = 1.0f;
        float                 m_glowAlpha = 0.0f;
        float                 m_pulseTimer = 0.0f;  // needed by render()
    };

} // namespace EC