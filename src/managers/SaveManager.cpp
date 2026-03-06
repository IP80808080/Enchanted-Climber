// ═══════════════════════════════════════════════════════════════
//  SaveManager.cpp  — simple JSON-style save using basic file I/O
//  No third-party JSON lib needed — we write/read key=value lines
// ═══════════════════════════════════════════════════════════════

#include "managers/SaveManager.h"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace EC {

void SaveManager::ensureSaveDir() {
    std::filesystem::create_directories("saves");
}

bool SaveManager::load(SaveData& out) {
    std::ifstream f(SAVE_FILE);
    if (!f.is_open()) {
        std::cout << "[SaveManager] No save file found — using defaults.\n";
        // Default: first character and theme unlocked
        out.unlockedChars[0]  = true;
        out.unlockedThemes[0] = true;
        return false;
    }

    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto sep = line.find('=');
        if (sep == std::string::npos) continue;
        std::string key = line.substr(0, sep);
        std::string val = line.substr(sep + 1);

        if      (key == "highScore")      out.highScore      = std::stoi(val);
        else if (key == "highestHeight")  out.highestHeight  = std::stof(val);
        else if (key == "totalStars")     out.totalStars     = std::stoi(val);
        else if (key == "totalJumps")     out.totalJumps     = std::stoi(val);
        else if (key == "totalPlatforms") out.totalPlatforms = std::stoi(val);
        else if (key == "musicVolume")    out.musicVolume    = std::stoi(val);
        else if (key == "sfxVolume")      out.sfxVolume      = std::stoi(val);
        else if (key == "activeTheme")    out.activeTheme    = (ThemeID)std::stoi(val);
        else if (key == "activeChar")     out.activeCharacter= (CharacterID)std::stoi(val);
        else if (key == "particlesOn")    out.particlesOn    = (val == "1");
        else if (key == "chars") {
            for (int i = 0; i < (int)CharacterID::COUNT && i < (int)val.size(); i++)
                out.unlockedChars[i] = (val[i] == '1');
        }
        else if (key == "themes") {
            for (int i = 0; i < (int)ThemeID::COUNT && i < (int)val.size(); i++)
                out.unlockedThemes[i] = (val[i] == '1');
        }
    }

    std::cout << "[SaveManager] Save loaded. High score: " << out.highScore << "\n";
    return true;
}

bool SaveManager::save(const SaveData& data) {
    ensureSaveDir();
    std::ofstream f(SAVE_FILE);
    if (!f.is_open()) {
        std::cerr << "[SaveManager] Could not write save file!\n";
        return false;
    }

    f << "# Enchanted Climber Save File\n";
    f << "highScore="      << data.highScore       << "\n";
    f << "highestHeight="  << data.highestHeight   << "\n";
    f << "totalStars="     << data.totalStars      << "\n";
    f << "totalJumps="     << data.totalJumps      << "\n";
    f << "totalPlatforms=" << data.totalPlatforms  << "\n";
    f << "musicVolume="    << data.musicVolume     << "\n";
    f << "sfxVolume="      << data.sfxVolume       << "\n";
    f << "activeTheme="    << (int)data.activeTheme    << "\n";
    f << "activeChar="     << (int)data.activeCharacter<< "\n";
    f << "particlesOn="    << (data.particlesOn ? "1" : "0") << "\n";

    f << "chars=";
    for (int i = 0; i < (int)CharacterID::COUNT; i++)
        f << (data.unlockedChars[i] ? "1" : "0");
    f << "\n";

    f << "themes=";
    for (int i = 0; i < (int)ThemeID::COUNT; i++)
        f << (data.unlockedThemes[i] ? "1" : "0");
    f << "\n";

    std::cout << "[SaveManager] Game saved.\n";
    return true;
}

} // namespace EC
