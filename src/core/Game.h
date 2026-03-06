#pragma once
// ═══════════════════════════════════════════════════════════════
//  EnchantedClimber — Game.h
//  All SDL3 — every library now uses the SDL3 subfolder convention:
//    SDL3       → <SDL3/SDL.h>
//    SDL3_image → <SDL3_image/SDL_image.h>
//    SDL3_ttf   → <SDL3_ttf/SDL_ttf.h>
//    SDL3_mixer → <SDL3_mixer/SDL_mixer.h>   ← fixed (was SDL2)
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>              // SDL3_mixer — subfolder header
#include <memory>
#include <stack>
#include "core/Types.h"
#include "core/Constants.h"

namespace EC {
    class GameState;
    class AssetManager;
    class AudioManager;
    class ThemeManager;
    class SaveManager;
    class AchievementManager;
    class ParticleSystem;
    class UIRenderer;
}

namespace EC {

    class Game {
    public:
        static Game& instance();

        bool init();
        void run();
        void shutdown();

        void pushState(std::unique_ptr<GameState> state);
        void popState();
        void changeState(std::unique_ptr<GameState> state);
        void clearAndPush(std::unique_ptr<GameState> state);

        SDL_Renderer* getRenderer() { return m_renderer; }
        SDL_Window* getWindow() { return m_window; }
        AssetManager& assets() { return *m_assets; }
        AudioManager& audio() { return *m_audio; }
        ThemeManager& themes() { return *m_themes; }
        SaveManager& saves() { return *m_saves; }
        AchievementManager& achievements() { return *m_achievements; }
        ParticleSystem& particles() { return *m_particles; }
        UIRenderer& ui() { return *m_uiRenderer; }
        InputState& input() { return m_input; }
        SaveData& saveData() { return m_saveData; }

        void requestQuit() { m_running = false; }
        bool isRunning() const { return m_running; }

    private:
        Game();
        ~Game();
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;

        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;

        std::unique_ptr<AssetManager>       m_assets;
        std::unique_ptr<AudioManager>       m_audio;
        std::unique_ptr<ThemeManager>       m_themes;
        std::unique_ptr<SaveManager>        m_saves;
        std::unique_ptr<AchievementManager> m_achievements;
        std::unique_ptr<ParticleSystem>     m_particles;
        std::unique_ptr<UIRenderer>         m_uiRenderer;

        std::stack<std::unique_ptr<GameState>> m_states;

        InputState m_input;
        SaveData   m_saveData;
        bool       m_running = false;

        enum class PendingAction { NONE, PUSH, POP, CHANGE, CLEAR_AND_PUSH };
        struct StateAction {
            PendingAction action = PendingAction::NONE;
            std::unique_ptr<GameState> state = nullptr;
        };
        StateAction m_pendingAction;

        void handlePendingAction();

        bool initSDL();
        bool initWindow();
        bool initManagers();
        void processEvents();
        void update(float dt);
        void render();
        void pollInput(const SDL_Event& e);
    };

} // namespace EC