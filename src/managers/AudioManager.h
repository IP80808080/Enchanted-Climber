#pragma once
// ═══════════════════════════════════════════════════════════════
//  AudioManager.h  —  SDL3_mixer (3.0) interface
//  Uses the fully new MIX_* API (no Mix_* compatibility).
//  ThemeID pulled from core/Types.h — no local redefinition.
// ═══════════════════════════════════════════════════════════════
#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "core/Types.h"

namespace EC {

    class AudioManager {
    public:
        AudioManager() = default;
        ~AudioManager();

        // ── Lifecycle ──────────────────────────────────────────
        bool init();   // call once after MIX_Init()

        // ── Asset loading ──────────────────────────────────────
        bool loadSFX  (const std::string& id, const std::string& path);
        bool loadMusic(const std::string& id, const std::string& path);

        // ── Playback ────────────────────────────────────────────
        void playSFX    (const std::string& id);
        void playMusic  (const std::string& id, bool loop = true);
        void stopMusic  ();
        void fadeToMusic(const std::string& id, int fadeMs = 1500);
        void playThemeMusic(ThemeID theme);

        // ── Volume ─────────────────────────────────────────────
        void setMusicVolume(int vol100);   // 0–100
        void setSFXVolume  (int vol100);   // 0–100

        MIX_Mixer* getMixer() { return m_mixer; }

    private:
        MIX_Mixer* m_mixer      = nullptr;   // output device
        MIX_Track* m_musicTrack = nullptr;   // one permanent music track

        // Pooled SFX tracks (fire-and-forget)
        static constexpr int SFX_TRACK_POOL = 8;
        MIX_Track* m_sfxTracks[SFX_TRACK_POOL] = {};
        int        m_sfxNext = 0;

        std::unordered_map<std::string, MIX_Audio*> m_sfx;
        std::unordered_map<std::string, MIX_Audio*> m_music;

        std::string m_currentMusic;
        float m_musicVol = 0.8f;   // 0..1
        float m_sfxVol   = 1.0f;   // 0..1
    };

} // namespace EC