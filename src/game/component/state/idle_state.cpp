//
// Created by wangm on 26-10-24.
//

#include "idle_state.h"

#include "climb_state.h"
#include "jump_state.h"
#include "walk_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"

namespace game {
namespace component {
namespace state {
void IdleState::enter() {
    playAnimation("idle");
}

void IdleState::exit() {}

std::unique_ptr<PlayerState> IdleState::handleInput(engine::core::Context& context) {
    auto& input_manager = context.getInputManager();
    // 接触爬梯，  move_up 切换到爬梯状态；
    if (player_component_.getPhysicsComponent().isCollidedLadder()) {
        if (input_manager.isActionDown(std::string{engine::utils::action::MOVE_UP})) {
            return std::make_unique<game::component::state::ClimbState>(player_component_);
        }
    }

    // 在梯子顶部  move_down 切换到爬梯状态
    if (player_component_.getPhysicsComponent().isOnTopLadder()) {
        if (input_manager.isActionDown(std::string{engine::utils::action::MOVE_DOWN})) {
            // 将角色位置向下移动一点；  确保下一帧能判定到梯子上；
            player_component_.getTransformComponent().translate({0, 2.0f});
            return std::make_unique<game::component::state::ClimbState>(player_component_);
        }
    }

    if (input_manager.isActionDown(std::string{engine::utils::action::MOVE_LEFT}) ||
        input_manager.isActionDown(std::string{engine::utils::action::MOVE_RIGHT})) {
        // 左右移动  切换到 walkState;
        return std::make_unique<WalkState>(player_component_);
    } else if (input_manager.isActionDown(std::string{engine::utils::action::JUMP})) {
        // 跳跃  切换到jumpState;
        return std::make_unique<JumpState>(player_component_);
    }
    return nullptr;
}

std::unique_ptr<PlayerState> IdleState::update(const float, engine::core::Context&) {
    auto& physics_component = player_component_.getPhysicsComponent();
    auto friction_factor = player_component_.getFrictionFactor();
    physics_component.setVelocityX(physics_component.getVelocity().x * friction_factor);

    if (!player_component_.is_on_ground()) {
        return std::make_unique<FallState>(player_component_);
    }
    return nullptr;
}
} // state
} // component
} // game
