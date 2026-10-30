//
// Created by wangm on 26-10-24.
//

#include "player_component.h"

#include "../../engine/log/my_log.h"
#include "../../engine/object/game_object.h"

#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/audio_component.h"
#include "state/dead_state.h"
#include "state/hurt_state.h"
#include "state/idle_state.h"

namespace game {
namespace component {
bool PlayerComponent::takeDamage(int damage) {
    if (is_dead_ || damage < 1) { return false; }
    if (!health_component_->takeDamage(damage)) { return false; }
    spdlog::info("hp = {}", health_component_->getCurrentHealth());
    if (health_component_->isAlive()) {
        // 没死亡  进入硬直；
        setState(std::make_unique<game::component::state::HurtState>(*this));
    } else {
        // 已经死亡  切换到死亡状态；
        is_dead_ = true;
        setState(std::make_unique<game::component::state::DeadState>(*this));
    }
    return true;
}

void PlayerComponent::setState(std::unique_ptr<game::component::state::PlayerState> new_state) {
    if (!new_state) { return; }
    if (current_state_) { current_state_->exit(); }
    current_state_ = std::move(new_state);
    mylog::MyLog{}.debug("player setState '{}'", typeid(*current_state_).name());
    current_state_->enter();
}

bool PlayerComponent::is_on_ground() const {
    return coyote_time_ > coyote_timer_ || physics_component_->isCollidedBelow();
}

void PlayerComponent::init() {
    if (!owner_) {
        mylog::MyLog{}.error("PlayerComponent init need owner!");
        return;
    }

    transform_component_ = owner_->getComponent<engine::component::TransformComponent>();
    physics_component_ = owner_->getComponent<engine::component::PhysicsComponent>();
    sprite_component_ = owner_->getComponent<engine::component::SpriteComponent>();
    animation_component_ = owner_->getComponent<engine::component::AnimationComponent>();
    health_component_ = owner_->getComponent<engine::component::HealthComponent>();
    audio_component_ = owner_->getComponent<engine::component::AudioComponent>();

    // 检查必要组件是否存在
    if (!transform_component_ || !physics_component_ || !sprite_component_ ||
        !animation_component_ || !health_component_ || !audio_component_) {
        mylog::MyLog{}.error("PlayerComponent missing component");
    }

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

    // Coyote time;
    if (!physics_component_->isCollidedBelow()) {
        coyote_timer_ += dt; // 离开地面开始计时；
    } else {
        coyote_timer_ = 0.0f;
    }

    // 无敌闪烁；
    if (health_component_->isInvincible()) {
        flash_timer_ += dt;
        if (flash_timer_ > 2 * flash_interval_) {
            flash_timer_ -= 2 * flash_interval_; // 闪烁计时器在 0-2倍闪烁间隔中循环；
        }
        if (flash_timer_ < flash_interval_) {
            sprite_component_->setIsHidden(true);
        } else {
            sprite_component_->setIsHidden(false);
        }
    } else {
        // 非无敌状态下   确保精灵可见；
        if (sprite_component_->isHidden()) {
            sprite_component_->setIsHidden(false);
        }
    }
}
} // component
} // game
