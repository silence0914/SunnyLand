//
// Created by wangm on 26-10-24.
//

#include "walk_state.h"
#include "fall_state.h"
#include "idle_state.h"
#include "jump_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/component/physics_component.h"

namespace game {
namespace component {
namespace state {
void WalkState::enter() {
    playAnimation("walk");
}

void WalkState::exit() {}

std::unique_ptr<PlayerState> WalkState::handleInput(engine::core::Context& context) {
    auto& input_manager = context.getInputManager();
    // 如果刚按下 ‘jump’ 就切换到 JumpState；
    if (input_manager.isActionDown(std::string{engine::utils::action::JUMP})) {
        return std::make_unique<JumpState>(player_component_);
    }

    // 步行状态可以左右移动；
    if (input_manager.isActionDown(std::string{engine::utils::action::MOVE_LEFT})) {
        moveLeft();
    } else if (input_manager.isActionDown(std::string{engine::utils::action::MOVE_RIGHT})) {
        moveRight();
    } else {
        // 如果没有移动  切换到 idleState;
        return std::make_unique<IdleState>(player_component_);
    }
    return nullptr;
}

std::unique_ptr<PlayerState> WalkState::update(const float, engine::core::Context&) {
    // 限制最大速度；
    auto& physics_component = player_component_.getPhysicsComponent();
    float max_speed = player_component_.getMaxSpeed();
    physics_component.setVelocityX(glm::clamp(physics_component.getVelocity().x, -max_speed, max_speed));

    // 如果下方没有碰撞；  切换到fallState
    if (!physics_component.isCollidedBelow()) {
        return std::make_unique<game::component::state::FallState>(player_component_);
    }
    return nullptr;
}
} // state
} // component
} // game
