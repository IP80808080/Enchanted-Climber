// ═══════════════════════════════════════════════════════════════
//  AssetManager.cpp
//  Loads & caches SDL textures and TTF fonts.
//  SDL3_image: IMG_LoadTexture still works the same way.
//  SDL3_ttf:   TTF_OpenFont still works the same way.
// ═══════════════════════════════════════════════════════════════

#include "managers/AssetManager.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

namespace EC {

AssetManager::AssetManager(SDL_Renderer* r)
    : m_renderer(r) {}

AssetManager::~AssetManager() {
    clearAll();
}

// ── getTexture ─────────────────────────────────────────────────
// Returns cached texture, or loads it fresh if not seen before.
SDL_Texture* AssetManager::getTexture(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end())
        return it->second;

    SDL_Texture* tex = IMG_LoadTexture(m_renderer, path.c_str());
    if (!tex) {
        std::cerr << "[AssetManager] Failed to load texture: "
                  << path << " — " << SDL_GetError() << "\n";
    }

    // Always cache the result, even if it's nullptr, to avoid re-spamming errors
    m_textures[path] = tex;
    return tex;
}

// ── getFont ────────────────────────────────────────────────────
TTF_Font* AssetManager::getFont(const std::string& path, int size) {
    auto it = m_fonts.find(path);
    if (it != m_fonts.end()) {
        auto it2 = it->second.find(size);
        if (it2 != it->second.end())
            return it2->second;
    }

    // SDL3_ttf: TTF_OpenFont signature unchanged
    TTF_Font* font = TTF_OpenFont(path.c_str(), (float)size);
    if (!font) {
        std::cerr << "[AssetManager] Failed to load font: "
                  << path << " size " << size
                  << " — " << SDL_GetError() << "\n";
        return nullptr;
    }

    m_fonts[path][size] = font;
    return font;
}

// ── clearAll ───────────────────────────────────────────────────
void AssetManager::clearAll() {
    for (auto& [path, tex] : m_textures)
        if (tex) SDL_DestroyTexture(tex);
    m_textures.clear();

    for (auto& [path, sizeMap] : m_fonts)
        for (auto& [size, font] : sizeMap)
            if (font) TTF_CloseFont(font);
    m_fonts.clear();
}

} // namespace EC
