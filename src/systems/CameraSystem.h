#pragma once
// SDL3 core → <SDL3/SDL.h>
#include <SDL3/SDL.h>
#include "core/Types.h"
namespace EC {
class Player;
class CameraSystem {
public:
    void  update  (const Player& player, float dt);
    float getCameraY() const { return m_cameraY + m_shakeY; }
    void  reset   ()         { m_cameraY = 0; m_targetY = 0; m_shakeTimer = 0; m_shakeY = 0; }
    void  shake   (float duration, float intensity) { m_shakeTimer = duration; m_shakeIntensity = intensity; }
private:
    float m_cameraY = 0.0f;
    float m_targetY = 0.0f;
    float m_shakeTimer = 0.0f;
    float m_shakeIntensity = 0.0f;
    float m_shakeY = 0.0f;
};
} // namespace EC
