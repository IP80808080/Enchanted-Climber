#include "rendering/AnimationSystem.h"
#include "core/Constants.h"
namespace EC {
void AnimationSystem::load(SDL_Renderer*, const std::string&) {}
void AnimationSystem::setState(AnimState s) { m_current = s; m_frame = 0; }
void AnimationSystem::update(float dt) {
    m_frameTimer += dt;
    if (m_frameTimer >= EC::ANIM_FRAME_DURATION) {
        m_frameTimer = 0;
        m_frame = (m_frame + 1) % 4;
    }
}
void AnimationSystem::render(SDL_Renderer*, const Rect&, bool) {}
} // namespace EC
