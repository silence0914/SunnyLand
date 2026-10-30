//
// Created by wangm on 26-10-26.
//

#include "hurt_state.h"

#include "fall_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "../player_component.h"
#include "../../../engine/component/audio_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"

namespace game {
namespace component {
namespace state {
void HurtState::enter() {
    stunned_timer_ = 0;
    playAnimation("hurt");
    auto& physics_component = player_component_.getPhysicsComponent();
    auto& sprite_component = player_component_.getSpriteComponent();

    // 默认朝左上方击退；
    auto knockback_velocity = glm::vec2{-100.0f, -150.0f};
    // 根据角色方向； 调整击退方向；
    if (sprite_component.isFlipped()) {
        knockback_velocity.x = -knockback_velocity.x;
    }
    physics_component.setVelocity(knockback_velocity);
    player_component_.getAudioComponent().playSound("hurt");
}

void HurtState::exit() {}

// 硬直状态 不能操作；
std::unique_ptr<PlayerState> HurtState::handleInput(engine::core::Context&) {
    return nullptr;
}

std::unique_ptr<PlayerState> HurtState::update(const float dt, engine::core::Context&) {
    stunned_timer_ += dt;
    auto& physics_component = player_component_.getPhysicsComponent();
    // 落地解除硬直
    if (physics_component.isCollidedBelow()) {
        if (glm::abs(physics_component.getVelocity().x) < 1.0f) {
            return std::make_unique<IdleState>(player_component_);
        } else {
            return std::make_unique<WalkState>(player_component_);
        }
    }
    if (stunned_timer_ > player_component_.getStunnedDuration()) {
        return std::make_unique<FallState>(player_component_);
    }
    return nullptr;
}
} // state
} // component
} // game
