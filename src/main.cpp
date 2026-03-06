// ═══════════════════════════════════════════════════════════════
//  EnchantedClimber — main.cpp
//  Entry point. SDL3 note: main() signature must be
//  int main(int argc, char* argv[]) for SDL3 to hook properly.
// ═══════════════════════════════════════════════════════════════

#include "core/Game.h"
#include <iostream>

int main(int /*argc*/, char* /*argv*/[]) {

    EC::Game& game = EC::Game::instance();

    if (!game.init()) {
        std::cerr << "Failed to initialise Enchanted Climber.\n";
        return 1;
    }

    game.run();

    return 0;
}