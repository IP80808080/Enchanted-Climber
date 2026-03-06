#pragma once
// ═══════════════════════════════════════════════════════════════
//  ThemeManager.h
//  SDL3 core → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "core/Types.h"
#include <array>

namespace EC {
class ThemeManager {
public:
    ThemeManager();
    void             setTheme(ThemeID id);
    const ThemeData& current()         const;
    const ThemeData& get(ThemeID id)   const;
    ThemeID          activeID()        const { return m_active; }

private:
    std::array<ThemeData, (int)ThemeID::COUNT> m_themes;
    ThemeID m_active = ThemeID::FAIRY_GARDEN;
    void buildThemes();
};
} // namespace EC
