#pragma once
// ═══════════════════════════════════════════════════════════════
//  SaveManager.h  — no SDL includes needed
// ═══════════════════════════════════════════════════════════════
#include "core/Types.h"
#include <string>

namespace EC {
class SaveManager {
public:
    bool load(SaveData& out);
    bool save(const SaveData& data);
private:
    const std::string SAVE_FILE = "saves/save.json";
    void ensureSaveDir();
};
} // namespace EC
