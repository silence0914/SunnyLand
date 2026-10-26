//
// Created by wangm on 26-10-25.
//

#include "fall_state.h"
#include "walk_state.h"
#include "idle_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/utils/action.h"
#include "../../../engine/component/physics_component.h"


namespace game {
namespace component {
namespace state {
void FallState::enter() {
    playAnimation("fall");
}

void FallState::exit() {}

std::unique_ptr<PlayerState> FallState::handleInput(engine::core::Context& context) {
    auto& input_manager = context.getInputManager();
    // 下落状态  可以左右移动
    if (input_manager.isActionDown(std::string{engine::utils::action::MOVE_LEFT})) {
        moveLeft();
    } else if (input_manager.isActionDown(std::string{engine::utils::action::MOVE_RIGHT})) {
        moveRight();
    }
    return nullptr;
}

std::unique_ptr<PlayerState> FallState::update(const float, engine::core::Context&) {
    auto& physics_component = player_component_.getPhysicsComponent();
    auto max_speed = player_component_.getMaxSpeed();
    physics_component.setVelocityX(glm::clamp(physics_component.getVelocity().x, -max_speed, max_speed));
    if (physics_component.isCollidedBelow()) {
        if (glm::abs(physics_component.getVelocity().x) < 1.0f) {
            return std::make_unique<IdleState>(player_component_);
        } else {
            return std::make_unique<WalkState>(player_component_);
        }
    }
    return nullptr;
}
} // state
} // component
} // game
