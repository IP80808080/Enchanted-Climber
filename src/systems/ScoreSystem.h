#pragma once
#include "core/Types.h"
namespace EC {
class ScoreSystem {
public:
    void reset();
    void start             (float startY);
    void update            (PlayerStats& stats, float playerY, float cameraY);
    void onLand            (PlayerStats& stats);
    void addPlatformScore  (PlayerStats& stats);   // alias used by PlayingState
    void onCollect         (PlayerStats& stats, int value);
    void onSpecialPlatform (PlayerStats& stats);
private:
    float m_lowestY = 99999.0f;
};
} // namespace EC
