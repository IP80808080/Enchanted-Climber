# ✨ Enchanted Climber

A magical-girl infinite vertical platformer built in C++ with SDL3. Jump between enchanted platforms, collect stars, trigger particle effects, and climb through 5 beautifully themed worlds.

---

## 🎮 Gameplay

- **Infinite vertical climbing** — platforms generate procedurally as you rise
- **5 magical themes** — Fairy Garden, Mermaid Lagoon, Princess Castle, Witch's Wonderland, Sweet Treats
- **5 playable characters** — each with unique idle/jump/fall animations
- **Platform types** — Normal, Moving, Breaking, Bouncy, Special
- **Collectibles** — Stars, Hearts, Crystals, Shields, Runes
- **Particle system** — petals, bubbles, sparkles, hearts per theme
- **Score & streak system** with HUD

---

## 🛠️ Tech Stack

| | |
|---|---|
| Language | C++17 |
| Window / Input | SDL3 |
| Sprites | SDL3_image |
| Text | SDL3_ttf |
| Audio | SDL3_mixer 3.0 |
| Build | CMake 3.10+ |
| Compiler | MSVC x64 (Visual Studio 2022) |

---

## 📁 Project Structure

```
EnchantedClimber/
├── src/
│   ├── core/          # Game loop, constants, shared types
│   ├── states/        # Menu, Play, Pause, GameOver, Shop, CharacterSelect
│   ├── entities/      # Player, Platform, Collectible
│   ├── systems/       # Physics, Collision, Camera, PlatformGen, Score
│   ├── rendering/     # Renderer, AnimationSystem, ParticleSystem, UIRenderer
│   ├── audio/         # AudioManager, SoundEffect
│   ├── managers/      # AssetManager, ThemeManager, SaveManager, AchievementManager
│   ├── themes/        # Per-theme data classes
│   └── ui/            # Button, Slider, HUD, Panel
├── assets/
│   ├── sprites/       # backgrounds, characters, platforms, particles, ui, collectibles
│   ├── audio/         # music (ogg) + sfx (wav)
│   └── fonts/         # magical_font.ttf, ui_font.ttf
├── external/          # SDL3, SDL3_image, SDL3_ttf, SDL3_mixer
├── data/              # themes.json, characters.json, achievements.json
├── docs/              # SRS, architecture docs
└── CMakeLists.txt
```

---

## 🔧 Building

### Prerequisites

- Visual Studio 2022 (with C++ Desktop workload)
- CMake 3.10 or later
- SDL3 libraries in `external/` (see below)

### SDL3 Libraries

Download and place into `external/` with this layout:

```
external/
├── SDL3/        include/ + lib/x64/SDL3.lib + SDL3.dll
├── SDL3_image/  include/ + lib/x64/SDL3_image.lib + SDL3_image.dll
├── SDL3_ttf/    include/ + lib/x64/SDL3_ttf.lib + SDL3_ttf.dll
└── SDL3_mixer/  include/ + lib/x64/SDL3_mixer.lib + SDL3_mixer.dll
```

| Library | Download |
|---|---|
| SDL3 | https://github.com/libsdl-org/SDL/releases |
| SDL3_image | https://github.com/libsdl-org/SDL_image/releases |
| SDL3_ttf | https://github.com/libsdl-org/SDL_ttf/releases |
| SDL3_mixer | https://github.com/libsdl-org/SDL_mixer/releases |

### Build Steps

```bash
# Clone
git clone https://github.com/YOUR_USERNAME/enchanted-climber.git
cd enchanted-climber

# Configure & build (Visual Studio)
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# Or open in VS: File → Open → CMake, select CMakeLists.txt
```

CMake automatically copies all DLLs and the `assets/` folder next to the `.exe` after every build.

---

## 🎵 Audio

SFX and music are loaded at startup via `AudioManager::loadAll()`. Files load silently if missing — the game runs fine without audio assets.

| ID | File |
|---|---|
| `jump` | assets/audio/sfx/jump.wav |
| `land` | assets/audio/sfx/land.wav |
| `collect` | assets/audio/sfx/collect.wav |
| `click` | assets/audio/sfx/click.wav |
| `hover` | assets/audio/sfx/hover.wav |
| `transform` | assets/audio/sfx/transform.wav |
| `gameover` | assets/audio/sfx/gameover.wav |
| `unlock` | assets/audio/sfx/unlock.wav |
| `powerup` | assets/audio/sfx/powerup.wav |
| `warning` | assets/audio/sfx/warning.wav |
| Theme music | assets/audio/music/\<theme\>.ogg |

---

## 🖼️ Asset Structure

All sprites are RGBA PNGs loaded on demand by `AssetManager::getTexture()`. Missing files fall back to procedural colored rectangles — nothing crashes.

| Folder | Contents |
|---|---|
| `sprites/backgrounds/` | 480×854 per-theme backgrounds |
| `sprites/characters/<name>/` | idle.png (4f), jump.png (2f), fall.png (2f) |
| `sprites/platforms/<theme>/` | platform, moving, breaking, bouncy, special |
| `sprites/particles/` | sparkle, heart, bubble, petal, star |
| `sprites/collectibles/` | star, crystal, heart, shield, rune |
| `sprites/ui/` | buttons, panels, bars, icons, arrows |

---

## 💾 Save System

Player progress is saved to `saves/save.json` at runtime. This file is git-ignored.

---

## 📜 License

Academic project — CDAC IACSD. Not for redistribution.
