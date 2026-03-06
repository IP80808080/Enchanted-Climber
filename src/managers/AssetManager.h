#pragma once
// ═══════════════════════════════════════════════════════════════
//  AssetManager.h
//  SDL3_image → <SDL3_image/SDL_image.h>
//  SDL3_ttf   → <SDL3_ttf/SDL_ttf.h>
//  SDL3 core  → <SDL3/SDL.h>
// ═══════════════════════════════════════════════════════════════
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>

namespace EC {
class AssetManager {
public:
    explicit AssetManager(SDL_Renderer* r);
    ~AssetManager();

    SDL_Texture* getTexture(const std::string& path);
    TTF_Font*    getFont(const std::string& path, int size);
    void         clearAll();

private:
    SDL_Renderer* m_renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*>                           m_textures;
    std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>>     m_fonts;
};
} // namespace EC
