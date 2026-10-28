//
// Created by wangm on 26-10-28.
//

#include "climb_state.h"

#include "fall_state.h"
#include "idle_state.h"
#include "jump_state.h"
#include "../player_component.h"
#include "../utils.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"

namespace game {
namespace component {
namespace state {
void ClimbState::enter() {
    playAnimation(toString(State::CLIMB));                        // 播放攀爬动画；
    player_component_.getPhysicsComponent().setUseGravity(false); // 禁用重力
}

void ClimbState::exit() {
    player_component_.getPhysicsComponent().setUseGravity(true); // 恢复重力
}

std::unique_ptr<PlayerState> ClimbState::handleInput(engine::core::Context& context) {
    auto& input_manager = context.getInputManager();
    auto& physics_engine = player_component_.getPhysicsComponent();
    auto& animation_component = player_component_.getAnimationComponent();

    // 攀爬状态下；  按键移动  不按禁止；
    auto is_up = input_manager.isActionDown(std::string{engine::utils::action::MOVE_UP});
    auto is_down = input_manager.isActionDown(std::string{engine::utils::action::MOVE_DOWN});
    auto is_left = input_manager.isActionDown(std::string{engine::utils::action::MOVE_LEFT});
    auto is_right = input_manager.isActionDown(std::string{engine::utils::action::MOVE_RIGHT});
    float speed = player_component_.getClimbSpeed();

    physics_engine.setVelocityX(is_left ? -speed : is_right ? speed : 0.0f);
    physics_engine.setVelocityY(is_up ? -speed : is_down ? speed : 0.0f);

    // 有移动就恢复动画；  没有就暂停；
    (is_up || is_down || is_left || is_right)
        ? animation_component.resumeAnimation()
        : animation_component.stopAnimation();

    if (input_manager.isActionDown(std::string{engine::utils::action::JUMP})) {
        return std::make_unique<game::component::state::JumpState>(player_component_);
    }
    return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::update(const float, engine::core::Context&) {
    auto& physics_component = player_component_.getPhysicsComponent();
    if (physics_component.isCollidedBelow()) {
        // 接触地面；  切换idle
        return std::make_unique<game::component::state::IdleState>(player_component_);
    } else if (!physics_component.isCollidedLadder()) {
        // 离开梯子但没有着地或者跳跃；  切换为滞空；
        return std::make_unique<game::component::state::FallState>(player_component_);
    }
    return nullptr;
}
} // state
} // component
} // game
