//
// Created by wangm on 26-10-24.
//

#include "player_component.h"

#include "../../engine/log/my_log.h"
#include "../../engine/object/game_object.h"

#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "state/idle_state.h"

namespace game {
namespace component {
void PlayerComponent::setState(std::unique_ptr<game::component::state::PlayerState> new_state) {
    if (!new_state) { return; }
    if (current_state_) { current_state_->exit(); }
    current_state_ = std::move(new_state);
    mylog::MyLog{}.debug("player setState '{}'", typeid(*current_state_).name());
    current_state_->enter();
}

void PlayerComponent::init() {
    if (!owner_) {
        mylog::MyLog{}.error("PlayerComponent init need owner!");
        return;
    }

    transform_component_ = owner_->getComponent<engine::component::TransformComponent>();
    physics_component_ = owner_->getComponent<engine::component::PhysicsComponent>();
    sprite_component_ = owner_->getComponent<engine::component::SpriteComponent>();

    // 初始化 状态机
    current_state_ = std::make_unique<game::component::state::IdleState>(*this);
    if (current_state_) {
        setState(std::move(current_state_));
    } else {
        mylog::MyLog{}.error("PlayerComponent init current_state_ is nullptr");
    }
}

void PlayerComponent::handleInput(engine::core::Context& context) {
    if (!current_state_)return;
    auto next_state = current_state_->handleInput(context);
    if (next_state) { setState(std::move(next_state)); }
}

void PlayerComponent::update(const float dt, engine::core::Context& context) {
    if (!current_state_) { return; }
    auto next_state = current_state_->update(dt, context);
    if (next_state) { setState(std::move(next_state)); }
}
} // component
} // game
