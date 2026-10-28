//
// Created by wangm on 26-10-28.
//

#include "updown_behavior.h"

#include "../ai_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/log/log_manager.h"
#include "../../../engine/log/my_log.h"

namespace game {
namespace component {
namespace ai {
UpdownBehavior::UpdownBehavior(float patrol_up, float patrol_down, float move_speed): patrol_up_(patrol_up),
                                                                                        patrol_down_(patrol_down),
                                                                                        move_speed_(move_speed) {
    if (patrol_up_ > patrol_down_) {
        mylog::MyLog{}.error("patrol_up '{}' > patrol_down '[]'", patrol_up_, patrol_down_);
        patrol_up_ = patrol_down_;
    }

    if (move_speed < 0) {
        move_speed_ = std::abs(move_speed);
    }
}

void UpdownBehavior::enter(game::component::AiComponent& ai_component) {
    ai_component.getPhysicsComponent().setVelocityY(moving_down_ ? move_speed_ : -move_speed_);
    ai_component.getAnimationComponent().playAnimation(toString(State::FLY));
    ai_component.getPhysicsComponent().setUseGravity(false);
}

void UpdownBehavior::update(const float x, game::component::AiComponent& ai_component) {
    auto& physics_component = ai_component.getPhysicsComponent();
    auto& transform_component = ai_component.getTransformComponent();
    auto current_y = transform_component.getPosition().y;
    if (physics_component.isCollidedAbove() || current_y < patrol_up_) {
        physics_component.setVelocityY(move_speed_);
        moving_down_ = true;
    } else if (physics_component.isCollidedBelow() || current_y > patrol_down_) {
        physics_component.setVelocityY(-move_speed_);
        moving_down_ = false;
    }
}
} // ai
} // component
} // game
