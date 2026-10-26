//
// Created by wangm on 26-10-25.
//

#include "player_state.h"
#include "../player_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/physics/physics_engine.h"

namespace game::component::state {
void PlayerState::playAnimation(const std::string& animation_name) {
    auto& animation_component = player_component_.getAnimationComponent();
    animation_component.playAnimation(animation_name);
}

void PlayerState::moveLeft() const {
    auto& physics_component = player_component_.getPhysicsComponent();
    if (physics_component.getVelocity().x > 0) {
        // 正在向右移动；
        physics_component.setVelocityX(0.0f);
    }
    physics_component.addForce({-player_component_.getMoveForce(), 0.0f});
    player_component_.getSpriteComponent().setIsFlipped(true);
}

void PlayerState::moveRight() const {
    auto& physics_component = player_component_.getPhysicsComponent();
    if (physics_component.getVelocity().x < 0) {
        // 正在向右移动；
        physics_component.setVelocityX(0.0f);
    }
    physics_component.addForce({player_component_.getMoveForce(), 0.0f});
    player_component_.getSpriteComponent().setIsFlipped(false);
}
}
