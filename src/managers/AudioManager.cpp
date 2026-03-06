// ═══════════════════════════════════════════════════════════════
//  AudioManager.cpp  —  SDL3_mixer 3.0
//  Full MIX_* implementation. All Mix_* calls removed.
//  Fixed multiple API errors: MIX_PlayTrack arguments, MIX_StopTrack.
// ═══════════════════════════════════════════════════════════════

#include "managers/AudioManager.h"
#include "core/Constants.h"
#include <iostream>

namespace EC {

    // ── Destructor ────────────────────────────────────────────────
    AudioManager::~AudioManager() {
        for (auto& pair : m_sfx) {
            if (pair.second) MIX_DestroyAudio(pair.second);
        }
        for (auto& pair : m_music) {
            if (pair.second) MIX_DestroyAudio(pair.second);
        }

        // Tracks are destroyed with the mixer
        if (m_mixer) MIX_DestroyMixer(m_mixer);
    }

    // ── init ───────────────────────────────────────────────────────
    bool AudioManager::init() {
        m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!m_mixer) {
            std::cerr << "[AudioManager] MIX_CreateMixerDevice failed: "
                      << SDL_GetError() << "\n";
            return false;  // non-fatal — game runs without audio
        }

        // One permanent track for background music
        m_musicTrack = MIX_CreateTrack(m_mixer);

        // Pre-create SFX track pool
        for (int i = 0; i < SFX_TRACK_POOL; i++)
            m_sfxTracks[i] = MIX_CreateTrack(m_mixer);

        std::cout << "[AudioManager] SDL3_mixer initialised OK.\n";
        return true;
    }

    // ── loadSFX ────────────────────────────────────────────────────
    bool AudioManager::loadSFX(const std::string& id, const std::string& path) {
        if (!m_mixer) return false;
        MIX_Audio* audio = MIX_LoadAudio(m_mixer, path.c_str(), /*predecode*/true);
        if (!audio) {
            std::cout << "[AudioManager] SFX not found (skipping): " << path << "\n";
            return false;
        }
        m_sfx[id] = audio;
        return true;
    }

    // ── loadMusic ──────────────────────────────────────────────────
    bool AudioManager::loadMusic(const std::string& id, const std::string& path) {
        if (!m_mixer) return false;
        MIX_Audio* audio = MIX_LoadAudio(m_mixer, path.c_str(), /*predecode*/false);
        if (!audio) {
            std::cout << "[AudioManager] Music not found (skipping): " << path << "\n";
            return false;
        }
        m_music[id] = audio;
        return true;
    }

    // ── playSFX ────────────────────────────────────────────────────
    void AudioManager::playSFX(const std::string& id) {
        if (!m_mixer) return;
        auto it = m_sfx.find(id);
        if (it == m_sfx.end() || !it->second) return;

        // Round-robin SFX track pool
        MIX_Track* track = m_sfxTracks[m_sfxNext];
        m_sfxNext = (m_sfxNext + 1) % SFX_TRACK_POOL;
        if (!track) return;

        MIX_StopTrack(track, 0); // Stop immediately if playing
        MIX_SetTrackAudio(track, it->second);
        MIX_SetTrackGain(track, m_sfxVol);
        
        // MIX_PlayTrack(track, options)
        MIX_PlayTrack(track, 0); 
    }

    // ── playMusic ──────────────────────────────────────────────────
    void AudioManager::playMusic(const std::string& id, bool loop) {
        if (!m_mixer || !m_musicTrack) return;
        auto it = m_music.find(id);
        if (it == m_music.end() || !it->second) return;
        if (m_currentMusic == id) return;

        MIX_StopTrack(m_musicTrack, 0);
        MIX_SetTrackAudio(m_musicTrack, it->second);
        MIX_SetTrackGain(m_musicTrack, m_musicVol);

        SDL_PropertiesID props = SDL_CreateProperties();
        if (loop) {
            SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        }
        MIX_PlayTrack(m_musicTrack, props);
        SDL_DestroyProperties(props);

        m_currentMusic = id;
    }

    // ── stopMusic ──────────────────────────────────────────────────
    void AudioManager::stopMusic() {
        if (m_musicTrack) MIX_StopTrack(m_musicTrack, 0);
        m_currentMusic = "";
    }

    // ── fadeToMusic ────────────────────────────────────────────────
    void AudioManager::fadeToMusic(const std::string& id, int fadeMs) {
        if (!m_mixer || !m_musicTrack) return;
        auto it = m_music.find(id);
        if (it == m_music.end() || !it->second) return;
        if (m_currentMusic == id) return;

        // In SDL3_mixer 3.0, MIX_PlayTrack with fade-in properties
        // will automatically handle the transition if the track is already playing.
        // However, for a clean switch, we can use the properties.

        SDL_PropertiesID props = SDL_CreateProperties();
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fadeMs);
        
        MIX_SetTrackAudio(m_musicTrack, it->second);
        MIX_SetTrackGain(m_musicTrack, m_musicVol);
        MIX_PlayTrack(m_musicTrack, props);
        SDL_DestroyProperties(props);

        m_currentMusic = id;
    }

    // ── volume ─────────────────────────────────────────────────────
    void AudioManager::setMusicVolume(int vol100) {
        m_musicVol = (float)vol100 / 100.0f;
        if (m_musicTrack) MIX_SetTrackGain(m_musicTrack, m_musicVol);
    }

    void AudioManager::setSFXVolume(int vol100) {
        m_sfxVol = (float)vol100 / 100.0f;
    }

    // ── playThemeMusic ─────────────────────────────────────────────
    void AudioManager::playThemeMusic(ThemeID theme) {
        static const char* ids[] = {
            "fairy_garden", "mermaid_lagoon", "princess_castle",
            "witch_wonderland", "sweet_treats"
        };
        int idx = static_cast<int>(theme);
        if (idx >= 0 && idx < 5)
            fadeToMusic(ids[idx], EC::MUSIC_FADE_MS);
    }

} // namespace EC