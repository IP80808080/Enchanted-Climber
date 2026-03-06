#pragma once
// ═══════════════════════════════════════════════════════════════
//  PlayingState.h
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include "states/GameState.h"
#include "entities/Player.h"
#include "entities/Platform.h"
#include "entities/Collectible.h"
#include "systems/PhysicsSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CameraSystem.h"
#include "systems/PlatformGenerator.h"
#include "systems/ScoreSystem.h"
#include "core/Types.h"
#include "core/Constants.h"
#include <array>

namespace EC {
    class PlayingState : public GameState {
    public:
        void onEnter()  override;
        void onExit()   override;
        void onPause()  override;
        void onResume() override;
        void handleEvent(const SDL_Event& e) override;
        void update(float dt) override;
        void render()         override;

    private:
        Player        m_player;
        std::array<Platform,   EC::PLATFORM_POOL_SIZE> m_platforms;
        std::array<Collectible, 20>                    m_collectibles;

        PhysicsSystem     m_physics;
        CollisionSystem   m_collision;
        CameraSystem      m_camera;
        PlatformGenerator m_generator;
        ScoreSystem       m_score;
        PlayerStats       m_stats;

        bool  m_gameOver         = false;
        float m_bgTimer          = 0.0f;
        float m_dangerAlpha      = 0.0f;
        float m_transitionTimer  = 0.0f;

        void checkGameOver();
        void spawnCollectibles(const Platform& p);
    };
} // namespace EC