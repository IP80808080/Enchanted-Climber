#include "systems/CameraSystem.h"
#include "entities/Player.h"
#include "core/Constants.h"
namespace EC {
void CameraSystem::update(const Player& player, float dt) {
    m_targetY = player.bounds.y - EC::CAMERA_LEAD;
    if (m_targetY < m_cameraY)
        m_cameraY += (m_targetY - m_cameraY) * EC::CAMERA_LERP_SPEED;

    // Shake logic
    if (m_shakeTimer > 0.0f) {
        m_shakeTimer -= dt;
        m_shakeY = (float)(rand() % 100 - 50) / 100.0f * m_shakeIntensity;
    } else {
        m_shakeY = 0.0f;
    }

    // Clamp camera so it doesn't look below the initial floor (assuming 0 is top of first screen)
    if (m_cameraY > 0) m_cameraY = 0;
}
} // namespace EC
