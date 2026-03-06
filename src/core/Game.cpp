// ═══════════════════════════════════════════════════════════════
//  EnchantedClimber — Game.cpp
//
//  SDL mix notes:
//    SDL3 core     → SDL3 API (bool returns, new event names)
//    SDL3_image    → NO IMG_Init/IMG_Quit — removed in SDL3_image 3.0
//    SDL3_ttf      → TTF_Init returns bool, float sizes
//    SDL3_mixer    → Full MIX_* API (no Mix_* compat shim)
// ═══════════════════════════════════════════════════════════════

#include "core/Game.h"
#include "core/Constants.h"

#include "states/GameState.h"
#include "states/MenuState.h"

#include "managers/AssetManager.h"
#include "managers/AudioManager.h"
#include "managers/ThemeManager.h"
#include "managers/SaveManager.h"
#include "managers/AchievementManager.h"

#include "rendering/ParticleSystem.h"
#include "rendering/UIRenderer.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <iostream>

namespace EC {

    Game& Game::instance() {
        static Game inst;
        return inst;
    }

    Game::Game() = default;
    Game::~Game() = default;

    // ── init ───────────────────────────────────────────────────────
    bool Game::init() {
        if (!initSDL())      return false;
        if (!initWindow())   return false;
        if (!initManagers()) return false;

        m_saves->load(m_saveData);

        // Apply saved theme
        m_themes->setTheme(m_saveData.activeTheme);
        m_audio->setMusicVolume(m_saveData.musicVolume);
        m_audio->setSFXVolume(m_saveData.sfxVolume);

        pushState(std::make_unique<MenuState>());

        m_running = true;
        std::cout << "[Game] Initialised — " << EC::WINDOW_TITLE << "\n";
        return true;
    }

    // ── run ────────────────────────────────────────────────────────
    void Game::run() {
        const float  FIXED_DT = EC::FIXED_TIMESTEP;
        Uint64       prevTime = SDL_GetPerformanceCounter();
        const Uint64 FREQ = SDL_GetPerformanceFrequency();
        float        accumulator = 0.0f;

        // Process the initial pushState(MenuState) from init()
        handlePendingAction();

        while (m_running && !m_states.empty()) {
            Uint64 now = SDL_GetPerformanceCounter();
            float  elapsed = static_cast<float>(now - prevTime) / static_cast<float>(FREQ);
            prevTime = now;
            if (elapsed > 0.25f) elapsed = 0.25f;
            accumulator += elapsed;

            processEvents();
            handlePendingAction();

            int steps = 0;
            while (accumulator >= FIXED_DT && steps < EC::MAX_FRAME_SKIP) {
                update(FIXED_DT);
                handlePendingAction();
                accumulator -= FIXED_DT;
                ++steps;
            }

            render();
            handlePendingAction();
        }
        shutdown();
    }

    // ── shutdown ───────────────────────────────────────────────────
    void Game::shutdown() {
        m_saves->save(m_saveData);
        while (!m_states.empty()) m_states.pop();

        m_uiRenderer.reset();
        m_particles.reset();
        m_achievements.reset();
        m_saves.reset();
        m_themes.reset();
        m_audio.reset();   // destroys mixer & all tracks
        m_assets.reset();

        if (m_renderer) { SDL_DestroyRenderer(m_renderer); m_renderer = nullptr; }
        if (m_window) { SDL_DestroyWindow(m_window);     m_window = nullptr; }

        // SDL3_ttf shutdown
        TTF_Quit();

        // SDL3_image: IMG_Quit() removed — no call needed

        // SDL3_mixer shutdown
        MIX_Quit();

        SDL_Quit();
        std::cout << "[Game] Shutdown complete.\n";
    }

    // ── State machine ──────────────────────────────────────────────
    void Game::pushState(std::unique_ptr<GameState> state) {
        m_pendingAction.action = PendingAction::PUSH;
        m_pendingAction.state  = std::move(state);
    }

    void Game::popState() {
        m_pendingAction.action = PendingAction::POP;
    }

    void Game::changeState(std::unique_ptr<GameState> state) {
        m_pendingAction.action = PendingAction::CHANGE;
        m_pendingAction.state  = std::move(state);
    }

    void Game::clearAndPush(std::unique_ptr<GameState> state) {
        m_pendingAction.action = PendingAction::CLEAR_AND_PUSH;
        m_pendingAction.state  = std::move(state);
    }

    void Game::handlePendingAction() {
        if (m_pendingAction.action == PendingAction::NONE) return;

        switch (m_pendingAction.action) {
            case PendingAction::PUSH:
                if (!m_states.empty()) m_states.top()->onPause();
                m_pendingAction.state->onEnter();
                m_states.push(std::move(m_pendingAction.state));
                break;

            case PendingAction::POP:
                if (!m_states.empty()) {
                    m_states.top()->onExit();
                    m_states.pop();
                }
                if (!m_states.empty()) m_states.top()->onResume();
                break;

            case PendingAction::CHANGE:
                if (!m_states.empty()) {
                    m_states.top()->onExit();
                    m_states.pop();
                }
                if (m_pendingAction.state) {
                    m_pendingAction.state->onEnter();
                    m_states.push(std::move(m_pendingAction.state));
                }
                break;

            case PendingAction::CLEAR_AND_PUSH:
                // Keep popping until empty
                while (!m_states.empty()) {
                    m_states.top()->onExit();
                    m_states.pop();
                }
                if (m_pendingAction.state) {
                    m_pendingAction.state->onEnter();
                    m_states.push(std::move(m_pendingAction.state));
                }
                break;
            default: break;
        }

        m_pendingAction.action = PendingAction::NONE;
        m_pendingAction.state.reset();
    }

    // ── initSDL ────────────────────────────────────────────────────
    bool Game::initSDL() {
        // SDL3 core: SDL_Init returns bool
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            std::cerr << "[SDL3] Init failed: " << SDL_GetError() << "\n";
            return false;
        }

        // SDL3_image 3.0: IMG_Init() and IMG_Quit() have been REMOVED.
        // Image format libraries are loaded automatically on first use.
        // No initialisation call needed.

        // SDL3_ttf: TTF_Init returns bool
        if (!TTF_Init()) {
            std::cerr << "[SDL3_ttf] Init failed: " << SDL_GetError() << "\n";
            return false;
        }

        // SDL3_mixer 3.0: MIX_Init (no arguments)
        if (!MIX_Init()) {
            std::cerr << "[SDL3_mixer] MIX_Init failed: " << SDL_GetError() << "\n";
            return false;
        }

        return true;
    }

    // ── initWindow ─────────────────────────────────────────────────
    bool Game::initWindow() {
        m_window = SDL_CreateWindow(
            EC::WINDOW_TITLE.c_str(),
            EC::SCREEN_WIDTH,
            EC::SCREEN_HEIGHT,
            SDL_WINDOW_RESIZABLE
        );
        if (!m_window) {
            std::cerr << "[SDL3] Window creation failed: " << SDL_GetError() << "\n";
            return false;
        }

        m_renderer = SDL_CreateRenderer(m_window, NULL);
        if (!m_renderer) {
            std::cerr << "[SDL3] Renderer creation failed: " << SDL_GetError() << "\n";
            return false;
        }

        SDL_SetRenderVSync(m_renderer, 1);
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

        // Set logical size so the game always renders 480x854
        SDL_SetRenderLogicalPresentation(m_renderer,
            EC::SCREEN_WIDTH, EC::SCREEN_HEIGHT,
            SDL_LOGICAL_PRESENTATION_LETTERBOX);

        return true;
    }

    // ── initManagers ───────────────────────────────────────────────
    bool Game::initManagers() {
        m_assets = std::make_unique<AssetManager>(m_renderer);
        m_audio = std::make_unique<AudioManager>();
        m_themes = std::make_unique<ThemeManager>();
        m_saves = std::make_unique<SaveManager>();
        m_achievements = std::make_unique<AchievementManager>();
        m_particles = std::make_unique<ParticleSystem>();
        m_uiRenderer = std::make_unique<UIRenderer>(m_renderer);

        if (!m_uiRenderer->loadFonts()) return false;

        // Initialize audio (creates MIX_Mixer device)
        m_audio->init();  // non-fatal if it fails

        // Load SFX — all non-fatal (game runs without audio)
        m_audio->loadSFX("jump", EC::SFX_PATH + "jump.wav");
        m_audio->loadSFX("land", EC::SFX_PATH + "land.wav");
        m_audio->loadSFX("collect", EC::SFX_PATH + "collect.wav");
        m_audio->loadSFX("gameover", EC::SFX_PATH + "gameover.wav");
        m_audio->loadSFX("hover", EC::SFX_PATH + "hover.wav");
        m_audio->loadSFX("click", EC::SFX_PATH + "click.wav");
        m_audio->loadSFX("unlock", EC::SFX_PATH + "unlock.wav");
        m_audio->loadSFX("powerup", EC::SFX_PATH + "powerup.wav");
        m_audio->loadSFX("warning", EC::SFX_PATH + "warning.wav");
        m_audio->loadSFX("transform", EC::SFX_PATH + "transform.wav");

        // Load music per theme
        m_audio->loadMusic("fairy_garden", EC::MUSIC_PATH + "fairy_garden.ogg");
        m_audio->loadMusic("mermaid_lagoon", EC::MUSIC_PATH + "mermaid_lagoon.ogg");
        m_audio->loadMusic("princess_castle", EC::MUSIC_PATH + "princess_castle.ogg");
        m_audio->loadMusic("witch_wonderland", EC::MUSIC_PATH + "witch_wonderland.ogg");
        m_audio->loadMusic("sweet_treats", EC::MUSIC_PATH + "sweet_treats.ogg");

        return true;
    }

    // ── processEvents ──────────────────────────────────────────────
    void Game::processEvents() {
        m_input.confirm = false;
        m_input.back = false;
        m_input.pause = false;
        m_input.quit = false;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                m_running = false;
                return;
            }
            pollInput(e);
            if (!m_states.empty())
                m_states.top()->handleEvent(e);
        }

        int         numKeys = 0;
        const bool* keys = SDL_GetKeyboardState(&numKeys);
        m_input.keys = keys;
        m_input.moveLeft = keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A];
        m_input.moveRight = keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D];
    }

    // ── pollInput ──────────────────────────────────────────────────
    void Game::pollInput(const SDL_Event& e) {
        if (e.type == SDL_EVENT_KEY_DOWN) {
            switch (e.key.scancode) {
            case SDL_SCANCODE_ESCAPE:
                m_input.back = true;
                m_input.pause = true;
                break;
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_SPACE:
                m_input.confirm = true;
                break;
            default: break;
            }
        }
    }

    // ── update / render ────────────────────────────────────────────
    void Game::update(float dt) {
        if (!m_states.empty()) m_states.top()->update(dt);
        m_particles->update(dt);
    }

    void Game::render() {
        // Default dark background before any state renders
        SDL_SetRenderDrawColor(m_renderer, 20, 10, 30, 255);
        SDL_RenderClear(m_renderer);
        if (!m_states.empty()) m_states.top()->render();
        SDL_RenderPresent(m_renderer);
    }

} // namespace EC