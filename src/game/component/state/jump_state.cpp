//
// Created by wangm on 26-10-24.
//

#include "jump_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/log/log_manager.h"
#include "../../../engine/log/my_log.h"

namespace game {
namespace component {
namespace state {
void JumpState::enter() {
    playAnimation("jump");
    auto& physics_component = player_component_.getPhysicsComponent();
    physics_component.setVelocityY(-player_component_.getJumpVel());
    mylog::MyLog{}.debug("playerComponent Jump;");
}

void JumpState::exit() {}

std::unique_ptr<PlayerState> JumpState::handleInput(engine::core::Context& context) {
    auto& input_manager = context.getInputManager();
    auto& physics_component = player_component_.getPhysicsComponent();
    auto& sprite_component = player_component_.getSpriteComponent();

    // 跳跃状态下可以左右移动；
    if (input_manager.isActionDown(std::string{engine::utils::action::MOVE_LEFT})) {
        if (physics_component.getVelocity().x > 0.0f) { physics_component.setVelocityX(0.0f); }
        physics_component.addForce({-player_component_.getMoveForce(), 0.0f});
        sprite_component.setIsFlipped(true);
    } else if (input_manager.isActionDown(std::string{engine::utils::action::MOVE_RIGHT})) {
        if (physics_component.getVelocity().x < 0.0f) { physics_component.setVelocityX(0.0f); }
        physics_component.addForce({player_component_.getMoveForce(), 0.0f});
        sprite_component.setIsFlipped(false);
    }
    return nullptr;
}

std::unique_ptr<PlayerState> JumpState::update(const float, engine::core::Context&) {
    // 限制最大速度
    auto& physics_component = player_component_.getPhysicsComponent();
    float max_speed = player_component_.getMaxSpeed();
    physics_component.setVelocityX(glm::clamp(physics_component.getVelocity().x, -max_speed, max_speed));

    // 如果速度为正；  切换到fall_state
    if (physics_component.getVelocity().y > 0.0f) {
        return std::make_unique<FallState>(player_component_);
    }
    return nullptr;
}
} // state
} // component
} // game
