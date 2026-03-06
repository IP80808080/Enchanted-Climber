#include "managers/ThemeManager.h"
#include "core/Constants.h"
namespace EC {
    ThemeManager::ThemeManager() { buildThemes(); }
    void ThemeManager::setTheme(ThemeID id) { m_active = id; }
    const ThemeData& ThemeManager::current() const { return m_themes[(int)m_active]; }
    const ThemeData& ThemeManager::get(ThemeID id) const { return m_themes[(int)id]; }
    void ThemeManager::buildThemes() {
        const std::string SP = EC::SPRITE_PATH;
        const std::string MUS = EC::AUDIO_PATH + "music/";
        auto& fg = m_themes[0];
        fg.id = ThemeID::FAIRY_GARDEN; fg.name = "fairy_garden"; fg.displayName = "Fairy Garden";
        fg.bgColorTop = { 255,182,220,255 }; fg.bgColorBottom = { 198,230,255,255 };
        fg.platformColor = { 255,220,240,255 }; fg.accentColor = { 255,140,200,255 }; fg.particleColor = { 255,200,230,255 };
        fg.backgroundSprite = SP + "backgrounds/fairy_garden.png";
        fg.platformSprite = SP + "platforms/fairy_garden/platform.png";
        fg.particleSprite = SP + "particles/petal.png";
        fg.musicFile = MUS + "fairy_garden.ogg";
        fg.particleType = ParticleType::PETAL;
        auto& ml = m_themes[1];
        ml.id = ThemeID::MERMAID_LAGOON; ml.name = "mermaid_lagoon"; ml.displayName = "Mermaid Lagoon";
        ml.bgColorTop = { 100,220,230,255 }; ml.bgColorBottom = { 30,100,180,255 };
        ml.platformColor = { 180,240,230,255 }; ml.accentColor = { 100,200,220,255 }; ml.particleColor = { 180,240,255,200 };
        ml.backgroundSprite = SP + "backgrounds/mermaid_lagoon.png";
        ml.platformSprite = SP + "platforms/mermaid_lagoon/platform.png";
        ml.particleSprite = SP + "particles/bubble.png";
        ml.musicFile = MUS + "mermaid_lagoon.ogg";
        ml.particleType = ParticleType::BUBBLE;
        auto& pc = m_themes[2];
        pc.id = ThemeID::PRINCESS_CASTLE; pc.name = "princess_castle"; pc.displayName = "Princess Castle";
        pc.bgColorTop = { 200,160,230,255 }; pc.bgColorBottom = { 255,210,230,255 };
        pc.platformColor = { 240,220,255,255 }; pc.accentColor = { 255,180,220,255 }; pc.particleColor = { 255,150,180,255 };
        pc.backgroundSprite = SP + "backgrounds/princess_castle.png";
        pc.platformSprite = SP + "platforms/princess_castle/platform.png";
        pc.particleSprite = SP + "particles/heart.png";
        pc.musicFile = MUS + "princess_castle.ogg";
        pc.particleType = ParticleType::HEART;
        auto& ww = m_themes[3];
        ww.id = ThemeID::WITCH_WONDERLAND; ww.name = "witch_wonderland"; ww.displayName = "Witch's Wonderland";
        ww.bgColorTop = { 30,10,60,255 }; ww.bgColorBottom = { 80,20,120,255 };
        ww.platformColor = { 120,50,180,255 }; ww.accentColor = { 200,100,255,255 }; ww.particleColor = { 220,180,255,255 };
        ww.backgroundSprite = SP + "backgrounds/witch_wonderland.png";
        ww.platformSprite = SP + "platforms/witch_wonderland/platform.png";
        ww.particleSprite = SP + "particles/sparkle.png";
        ww.musicFile = MUS + "witch_wonderland.ogg";
        ww.particleType = ParticleType::STAR;
        auto& st = m_themes[4];
        st.id = ThemeID::SWEET_TREATS; st.name = "sweet_treats"; st.displayName = "Sweet Treats";
        st.bgColorTop = { 255,230,180,255 }; st.bgColorBottom = { 255,180,200,255 };
        st.platformColor = { 255,220,160,255 }; st.accentColor = { 255,140,160,255 }; st.particleColor = { 255,200,100,255 };
        st.backgroundSprite = SP + "backgrounds/sweet_treats.png";
        st.platformSprite = SP + "platforms/sweet_treats/platform.png";
        st.particleSprite = SP + "particles/star.png";
        st.musicFile = MUS + "sweet_treats.ogg";
        st.particleType = ParticleType::CANDY;
    }
} // namespace EC