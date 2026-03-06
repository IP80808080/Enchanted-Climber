#pragma once

// ═══════════════════════════════════════════════════════════════
//  EnchantedClimber — Constants.h
//  Every tuneable value lives HERE. Never hardcode numbers elsewhere.
//  Updated for SDL3.
// ═══════════════════════════════════════════════════════════════

#include <string>

namespace EC {

    // ─────────────────────────────────────────────
    //  Window & Display
    // ─────────────────────────────────────────────
    constexpr int   SCREEN_WIDTH = 480;
    constexpr int   SCREEN_HEIGHT = 854;   // 9:16 portrait
    constexpr int   TARGET_FPS = 60;
    constexpr float FIXED_TIMESTEP = 1.0f / TARGET_FPS;
    constexpr int   MAX_FRAME_SKIP = 5;
    const std::string WINDOW_TITLE = "Enchanted Climber";

    // ─────────────────────────────────────────────
    //  Player Physics
    // ─────────────────────────────────────────────
    constexpr float GRAVITY = 0.5f;
    constexpr float JUMP_FORCE = -13.5f;
    constexpr float TERMINAL_VELOCITY = 15.0f;
    constexpr float MOVE_SPEED = 5.2f;
    constexpr float PLAYER_WIDTH = 48.0f;
    constexpr float PLAYER_HEIGHT = 64.0f;

    // ─────────────────────────────────────────────
    //  Platform Generation
    // ─────────────────────────────────────────────
    constexpr float PLATFORM_MIN_WIDTH = 60.0f;
    constexpr float PLATFORM_MAX_WIDTH = 180.0f;
    constexpr float PLATFORM_HEIGHT = 18.0f;
    constexpr float PLATFORM_GAP_MIN = 80.0f;
    constexpr float PLATFORM_GAP_MAX = 145.0f;
    constexpr int   PLATFORM_POOL_SIZE = 30;
    constexpr float SPECIAL_PLATFORM_CHANCE = 0.10f;
    constexpr float MAX_JUMP_HEIGHT = 180.0f;
    constexpr float MAX_JUMP_DISTANCE = 250.0f;

    // ─────────────────────────────────────────────
    //  Camera
    // ─────────────────────────────────────────────
    constexpr float CAMERA_LEAD = SCREEN_HEIGHT / 3.0f;
    constexpr float CAMERA_LERP_SPEED = 0.08f;

    // ─────────────────────────────────────────────
    //  Scoring
    // ─────────────────────────────────────────────
    constexpr float  SCORE_PER_PIXEL = 0.1f;
    constexpr int    SCORE_PER_STAR = 50;
    constexpr int    SCORE_PER_SPECIAL = 100;
    constexpr float  STREAK_BONUS = 0.10f;
    constexpr float  STREAK_BONUS_MAX = 2.0f;
    constexpr int    STREAK_BONUS_STEP = 10;
    constexpr float  NO_MISS_BONUS = 0.20f;

    // ─────────────────────────────────────────────
    //  Particles
    // ─────────────────────────────────────────────
    constexpr int   PARTICLE_POOL_SIZE = 300;
    constexpr int   JUMP_PARTICLES = 8;
    constexpr int   LAND_PARTICLES = 12;
    constexpr int   COLLECT_PARTICLES = 15;
    constexpr int   TRANSFORM_PARTICLES = 80;
    constexpr int   HIGHSCORE_PARTICLES = 60;
    constexpr float PARTICLE_GRAVITY = 0.15f;

    // ─────────────────────────────────────────────
    //  Audio  (SDL3_mixer uses 0-128 volume scale)
    // ─────────────────────────────────────────────
    constexpr int   AUDIO_FREQUENCY = 44100;
    constexpr int   AUDIO_CHANNELS = 2;
    constexpr int   AUDIO_CHUNK_SIZE = 2048;
    constexpr int   MIX_CHANNEL_COUNT = 16;
    constexpr int   DEFAULT_MUSIC_VOLUME = 80;
    constexpr int   DEFAULT_SFX_VOLUME = 100;
    constexpr int   MUSIC_FADE_MS = 1500;

    // ─────────────────────────────────────────────
    //  Animation
    // ─────────────────────────────────────────────
    constexpr int   ANIM_FRAMES_IDLE = 4;
    constexpr int   ANIM_FRAMES_JUMP = 4;
    constexpr int   ANIM_FRAMES_FALL = 4;
    constexpr int   ANIM_FRAMES_LAND = 3;
    constexpr float ANIM_FRAME_DURATION = 0.12f;

    // ─────────────────────────────────────────────
    //  Difficulty Scaling
    // ─────────────────────────────────────────────
    constexpr float DIFFICULTY_HEIGHT_STEP = 500.0f;
    constexpr float DIFFICULTY_GAP_INCREASE = 5.0f;
    constexpr float DIFFICULTY_SPEED_MULT = 1.05f;
    constexpr float DIFFICULTY_MAX_MULT = 2.5f;

    // ─────────────────────────────────────────────
    //  UI
    // ─────────────────────────────────────────────
    constexpr int   BUTTON_WIDTH = 200;
    constexpr int   BUTTON_HEIGHT = 56;
    constexpr int   FONT_SIZE_LARGE = 36;
    constexpr int   FONT_SIZE_MEDIUM = 24;
    constexpr int   FONT_SIZE_SMALL = 16;
    constexpr float BUTTON_HOVER_SCALE = 1.06f;

    // ─────────────────────────────────────────────
    //  Asset Paths  (relative to executable)
    // ─────────────────────────────────────────────
    const std::string ASSET_ROOT = "assets/";
    const std::string SPRITE_PATH = ASSET_ROOT + "sprites/";
    const std::string AUDIO_PATH = ASSET_ROOT + "audio/";
    const std::string MUSIC_PATH = AUDIO_PATH + "music/";
    const std::string SFX_PATH = AUDIO_PATH + "sfx/";
    const std::string FONT_PATH = ASSET_ROOT + "fonts/";
    const std::string DATA_PATH = "data/";
    const std::string SAVE_PATH = "saves/save.json";

    // ─────────────────────────────────────────────
    //  Collision
    // ─────────────────────────────────────────────
    constexpr float LAND_TOLERANCE = 12.0f;

} // namespace EC