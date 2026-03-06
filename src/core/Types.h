#pragma once

// ═══════════════════════════════════════════════════════════════
//  EnchantedClimber — Types.h
//  Shared data structures & enums used across all modules.
//  Updated for SDL3: SDL_Rect → SDL_FRect, SDL_Color unchanged.
// ═══════════════════════════════════════════════════════════════

#include <string>
#include <array>
#include <SDL3/SDL.h>

namespace EC {

    // ─────────────────────────────────────────────
    //  Game States
    // ─────────────────────────────────────────────
    enum class GameStateID {
        NONE,
        MENU,
        CHARACTER_SELECT,
        PLAYING,
        PAUSED,
        GAME_OVER,
        SHOP
    };

    // ─────────────────────────────────────────────
    //  Themes
    // ─────────────────────────────────────────────
    enum class ThemeID {
        FAIRY_GARDEN = 0,
        MERMAID_LAGOON,
        PRINCESS_CASTLE,
        WITCH_WONDERLAND,
        SWEET_TREATS,
        COUNT
    };

    // ─────────────────────────────────────────────
    //  Characters
    // ─────────────────────────────────────────────
    enum class CharacterID {
        LILY_FAIRY = 0,
        MARINA_MERMAID,
        ROSALIND_PRINCESS,
        MORGANA_WITCH,
        CHLOE_BAKER,
        AURORA_BUTTERFLY,
        STELLA_STARLIGHT,
        LUNA_MOONRABBIT,
        COUNT
    };

    // ─────────────────────────────────────────────
    //  Platform types
    // ─────────────────────────────────────────────
    enum class PlatformType {
        NORMAL,
        MOVING,
        BREAKING,
        BOUNCY,
        SPECIAL
    };

    // ─────────────────────────────────────────────
    //  Particle types
    // ─────────────────────────────────────────────
    enum class ParticleType {
        SPARKLE,
        PETAL,
        BUBBLE,
        HEART,
        STAR,
        CONFETTI,
        CANDY,
        MAGIC_DUST
    };

    // ─────────────────────────────────────────────
    //  Animation states
    // ─────────────────────────────────────────────
    enum class AnimState {
        IDLE,
        JUMPING,
        FALLING,
        LANDING
    };

    // ─────────────────────────────────────────────
    //  Vec2 — lightweight 2D vector
    // ─────────────────────────────────────────────
    struct Vec2 {
        float x = 0.0f;
        float y = 0.0f;

        Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
        Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
        Vec2 operator*(float s)       const { return { x * s,   y * s }; }
        Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
        Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    };

    // ─────────────────────────────────────────────
    //  Rect — axis-aligned rectangle (float)
    //  SDL3 uses SDL_FRect (float) natively — perfect match.
    // ─────────────────────────────────────────────
    struct Rect {
        float x = 0.0f, y = 0.0f;
        float w = 0.0f, h = 0.0f;

        float left()    const { return x; }
        float right()   const { return x + w; }
        float top()     const { return y; }
        float bottom()  const { return y + h; }
        float centerX() const { return x + w * 0.5f; }
        float centerY() const { return y + h * 0.5f; }

        // SDL3 uses SDL_FRect for rendering (float-based)
        SDL_FRect toSDL() const {
            return { x, y, w, h };
        }

        // AABB overlap test
        bool overlaps(const Rect& o) const {
            return left() < o.right() &&
                right() > o.left() &&
                top() < o.bottom() &&
                bottom() > o.top();
        }
    };

    // ─────────────────────────────────────────────
    //  Color — RGBA helper
    // ─────────────────────────────────────────────
    struct Color {
        Uint8 r = 255, g = 255, b = 255, a = 255;

        SDL_Color toSDL() const { return { r, g, b, a }; }

        static Color lerp(const Color& from, const Color& to, float t) {
            return {
                (Uint8)(from.r + (to.r - from.r) * t),
                (Uint8)(from.g + (to.g - from.g) * t),
                (Uint8)(from.b + (to.b - from.b) * t),
                (Uint8)(from.a + (to.a - from.a) * t)
            };
        }
    };

    // ─────────────────────────────────────────────
    //  ThemeData
    // ─────────────────────────────────────────────
    struct ThemeData {
        ThemeID         id;
        std::string     name;
        std::string     displayName;

        Color           bgColorTop;
        Color           bgColorBottom;
        Color           platformColor;
        Color           accentColor;
        Color           particleColor;

        std::string     backgroundSprite;
        std::string     platformSprite;
        std::string     musicFile;
        std::string     particleSprite;

        ParticleType    particleType;
    };

    // ─────────────────────────────────────────────
    //  CharacterData
    // ─────────────────────────────────────────────
    struct CharacterData {
        CharacterID     id;
        std::string     name;
        std::string     displayName;
        ThemeID         preferredTheme;
        std::string     spritePath;

        std::string     abilityName;
        std::string     abilityDesc;

        bool            isUnlocked = false;
        int             unlockStars = 0;
        float           unlockHeight = 0.0f;
    };

    // ─────────────────────────────────────────────
    //  Particle
    // ─────────────────────────────────────────────
    struct Particle {
        Vec2        position;
        Vec2        velocity;
        Color       color;
        float       lifetime = 0.0f;
        float       maxLifetime = 0.5f;
        float       size = 6.0f;
        float       rotation = 0.0f;
        float       rotSpeed = 0.0f;
        ParticleType type = ParticleType::SPARKLE;
        bool        active = false;
    };

    // ─────────────────────────────────────────────
    //  InputState — keyboard snapshot each frame
    //  SDL3: SDL_GetKeyboardState returns const bool*
    // ─────────────────────────────────────────────
    struct InputState {
        bool moveLeft = false;
        bool moveRight = false;
        bool pause = false;
        bool confirm = false;
        bool back = false;
        bool quit = false;

        const bool* keys = nullptr;  // SDL3: bool* (was Uint8* in SDL2)
    };

    // ─────────────────────────────────────────────
    //  PlayerStats
    // ─────────────────────────────────────────────
    struct PlayerStats {
        float   height = 0.0f;
        int     score = 0;
        int     stars = 0;
        int     platformsLanded = 0;
        int     streakCount = 0;
        float   scoreMultiplier = 1.0f;
        bool    isAlive = true;
    };

    // ─────────────────────────────────────────────
    //  SaveData
    // ─────────────────────────────────────────────
    struct SaveData {
        std::string     playerName = "Magical Girl";
        float           totalPlayTime = 0.0f;
        int             highScore = 0;
        float           highestHeight = 0.0f;
        int             totalStars = 0;
        int             totalJumps = 0;
        int             totalPlatforms = 0;

        bool unlockedChars[(int)CharacterID::COUNT] = {};
        bool unlockedThemes[(int)ThemeID::COUNT] = {};

        int             musicVolume = 80;
        int             sfxVolume = 100;
        ThemeID         activeTheme = ThemeID::FAIRY_GARDEN;
        CharacterID     activeCharacter = CharacterID::LILY_FAIRY;
        bool            particlesOn = true;
        bool            fullscreen = false;
    };

} // namespace EC