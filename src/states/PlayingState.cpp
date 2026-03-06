// ═══════════════════════════════════════════════════════════════
//  PlayingState.cpp  — Full enchanted gameplay loop
// ═══════════════════════════════════════════════════════════════
#include "states/PlayingState.h"
#include "core/Game.h"
#include "states/PausedState.h"
#include "states/GameOverState.h"
#include "rendering/UIRenderer.h"
#include "managers/AudioManager.h"
#include "managers/ThemeManager.h"
#include "rendering/ParticleSystem.h"
#include <cmath>
#include <algorithm>

namespace EC {

    // ── onEnter ────────────────────────────────────────────────────
    void PlayingState::onEnter() {
        Game& g = Game::instance();

        // Reset all state
        m_camera.reset();
        m_score.reset();
        m_stats = PlayerStats{};
        m_gameOver = false;
        m_bgTimer  = 0.0f;
        m_dangerAlpha = 0.0f;
        m_transitionTimer = 0.0f;

        // Re-init platforms (clear pool)
        for (auto& p : m_platforms) p.active = false;

        // Platform spawn height
        float platformY = (float)EC::SCREEN_HEIGHT * 0.65f;

        // Player spawn - placed exactly on top of the initial platform
        m_player.init(g.saveData().activeCharacter);
        m_player.bounds = {
            (float)EC::SCREEN_WIDTH / 2.0f - EC::PLAYER_WIDTH / 2.0f,
            platformY - EC::PLAYER_HEIGHT,
            (float)EC::PLAYER_WIDTH, (float)EC::PLAYER_HEIGHT
        };
        m_score.start(m_player.bounds.y);

        // Generator reset
        ThemeID tid = g.themes().activeID();
        m_generator.init(tid);

        // Generate initial island of platforms at spawn height
        m_platforms[0].reset(
            EC::SCREEN_WIDTH / 2.0f - 60.0f,
            platformY,
            120.0f, PlatformType::NORMAL);
        m_platforms[0].theme = tid;

        // Play music
        g.audio().playThemeMusic(tid);

        // Spawn entrance particles
        g.particles().spawnTransform(
            { (float)EC::SCREEN_WIDTH / 2.0f, (float)EC::SCREEN_HEIGHT / 2.0f },
            tid);
    }

    void PlayingState::onExit() {
        Game::instance().particles().clear();
    }

    void PlayingState::onPause()  {}
    void PlayingState::onResume() {}

    // ── handleEvent ────────────────────────────────────────────────
    void PlayingState::handleEvent(const SDL_Event& e) {
        if (e.type == SDL_EVENT_KEY_DOWN &&
            e.key.scancode == SDL_SCANCODE_ESCAPE)
            Game::instance().pushState(std::make_unique<PausedState>());
    }

    // ── update ────────────────────────────────────────────────────
    void PlayingState::update(float dt) {
        if (m_gameOver) return;

        Game& g = Game::instance();
        auto& input = g.input();

        m_bgTimer += dt;
        m_transitionTimer += dt;

        // ── Player horizontal movement
        if      (input.moveLeft)  m_player.velocity.x = -EC::MOVE_SPEED;
        else if (input.moveRight) m_player.velocity.x =  EC::MOVE_SPEED;
        else                      m_player.velocity.x =  0.0f;

        // ── Physics
        m_physics.update(m_player, dt);
        m_player.update(dt);

        // ── Collision detection
        bool landed = m_collision.checkPlayerPlatforms(
            m_player, m_platforms, m_stats);

        if (landed) {
            // Apply bounce force immediately
            m_player.applyJump();

            // Particles on landing
            g.particles().spawnLand(
                { m_player.bounds.centerX(), m_player.bounds.bottom() },
                g.themes().activeID());
            g.audio().playSFX("land");
            m_camera.shake(0.12f, 6.0f);

            // Scoring per platform
            m_score.addPlatformScore(m_stats);

            // Global stats
            g.saveData().totalJumps++;
            g.saveData().totalPlatforms++;
        }

        // ── Camera
        m_camera.update(m_player, dt);

        // ── Platform + collectible generation
        float diffScale = 1.0f + m_stats.height / (EC::DIFFICULTY_HEIGHT_STEP * 10.0f);
        diffScale = std::min(diffScale, EC::DIFFICULTY_MAX_MULT);
        m_generator.update(m_platforms, m_camera.getCameraY(), diffScale);

        // ── Platform updates
        for (auto& p : m_platforms)
            if (p.active) p.update(dt);

        // ── Collectible updates
        for (auto& c : m_collectibles) {
            if (!c.active) continue;
            c.update(dt);
            // Check player collision with star
            if (c.bounds.overlaps(m_player.bounds)) {
                c.active = false;
                m_stats.stars++;
                m_stats.score += EC::SCORE_PER_STAR;
                g.saveData().totalStars++;
                g.particles().spawnCollect(
                    { c.bounds.centerX(), c.bounds.centerY() },
                    g.themes().activeID());
                g.audio().playSFX("collect");
                m_camera.shake(0.08f, 4.0f);
            }
            // Deactivate if off screen below
            if (c.bounds.top() > m_camera.getCameraY() + EC::SCREEN_HEIGHT + 100)
                c.active = false;
        }

        // ── Score update (height-based)
        m_score.update(m_stats, m_player.bounds.y, m_camera.getCameraY());

        // ── Danger zone warning (player approaches bottom of screen)
        float playerScreenY = m_player.bounds.top() - m_camera.getCameraY();
        float dangerThreshold = EC::SCREEN_HEIGHT * 0.85f;
        if (playerScreenY > dangerThreshold) {
            float t = (playerScreenY - dangerThreshold) /
                      (EC::SCREEN_HEIGHT * 0.15f);
            m_dangerAlpha = std::min(1.0f, t);
        } else {
            m_dangerAlpha = std::max(0.0f, m_dangerAlpha - dt * 3.0f);
        }

        // ── Theme auto-change at milestones
        int themeZone = (int)(m_stats.height / 5000.0f) % (int)ThemeID::COUNT;
        if (themeZone != (int)g.themes().activeID()) {
            ThemeID newTheme = (ThemeID)themeZone;
            g.themes().setTheme(newTheme);
            g.audio().playThemeMusic(newTheme);
            g.particles().spawnTransform(
                { (float)EC::SCREEN_WIDTH / 2.0f,
                  m_player.bounds.centerY() - m_camera.getCameraY() },
                newTheme);
        }

        checkGameOver();
    }

    // ── render ────────────────────────────────────────────────────
    void PlayingState::render() {
        Game& g   = Game::instance();
        auto* r   = g.getRenderer();
        const ThemeData& theme = g.themes().current();

        // ── Animated layered background
        g.ui().drawAnimatedBackground(theme, m_bgTimer);

        float camY = m_camera.getCameraY();

        // ── Platforms
        for (auto& p : m_platforms)
            if (p.active) p.render(r, camY);

        // ── Collectibles
        for (auto& c : m_collectibles)
            if (c.active) c.render(r, camY);

        // ── Player
        m_player.render(r, camY);

        // ── World-space particles
        g.particles().renderWithCamera(r, camY);

        // ── Danger vignette (red pulsing edge glow)
        if (m_dangerAlpha > 0.01f) {
            float pulse = sinf(m_bgTimer * 8.0f) * 0.3f + 0.7f;
            g.ui().drawVignette(m_dangerAlpha * pulse, { 255, 60, 60, 255 });
        }

        // ── HUD (always last, on top)
        g.ui().drawHUD(m_stats, theme);

        // ── Entrance transition fade-in
        if (m_transitionTimer < 0.8f) {
            Uint8 fadeA = (Uint8)(255 * (1.0f - m_transitionTimer / 0.8f));
            SDL_SetRenderDrawColor(r, 0, 0, 0, fadeA);
            SDL_FRect full = { 0, 0, (float)EC::SCREEN_WIDTH, (float)EC::SCREEN_HEIGHT };
            SDL_RenderFillRect(r, &full);
        }
    }

    // ── checkGameOver ─────────────────────────────────────────────
    void PlayingState::checkGameOver() {
        float camBottom = m_camera.getCameraY() + EC::SCREEN_HEIGHT;
        if (m_player.bounds.top() > camBottom + 80.0f) {
            m_gameOver = true;

            // Update global save data
            Game& g = Game::instance();
            auto& save = g.saveData();
            if (m_stats.score > save.highScore) {
                save.highScore = m_stats.score;
                save.highestHeight = m_stats.height;
            }
            save.totalStars += m_stats.stars;

            g.audio().playSFX("gameover");
            g.changeState(std::make_unique<GameOverState>(m_stats));
        }
    }

    // ── spawnCollectibles ─────────────────────────────────────────
    void PlayingState::spawnCollectibles(const Platform& p) {
        if (!p.active) return;
        // 30% chance of star on platform
        if ((rand() % 10) < 3) {
            for (auto& c : m_collectibles) {
                if (!c.active) {
                    c.init(CollectibleType::STAR,
                           p.bounds.centerX(),
                           p.bounds.top() - 20.0f);
                    break;
                }
            }
        }
    }

} // namespace EC