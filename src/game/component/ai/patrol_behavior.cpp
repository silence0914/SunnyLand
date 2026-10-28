//
// Created by wangm on 26-10-27.
//

#include "patrol_behavior.h"

#include "../ai_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/log/my_log.h"

namespace game {
namespace component {
namespace ai {
PatrolBehavior::PatrolBehavior(float patrol_left, float patrol_right, float move_speed): patrol_left_(patrol_left),
                                                                                         patrol_right_(patrol_right),
                                                                                         move_speed_(move_speed) {
    if (patrol_left_ > patrol_right_) {
        mylog::MyLog{}.error("patrol_left > patrol_right");
        patrol_left_ = patrol_right_;
    }
}

void PatrolBehavior::enter(game::component::AiComponent& ai_component) {
    // 播放动画
    ai_component.getAnimationComponent().playAnimation(toString(State::WALK));
    ai_component.getPhysicsComponent().setVelocityX(moving_right_ ? move_speed_ : -move_speed_);
}

void PatrolBehavior::update(const float x, game::component::AiComponent& ai_component) {
    auto& physics_component = ai_component.getPhysicsComponent();
    auto& sprite_component = ai_component.getSpriteComponent();
    auto& transform_component = ai_component.getTransformComponent();

    auto current_x = transform_component.getPosition().x;
    if (physics_component.isCollidedRight() || current_x > patrol_right_) {
        physics_component.setVelocityX(-move_speed_); // 撞墙/到达巡逻边界 转向；
        moving_right_ = false;
    } else if (physics_component.isCollidedLeft() || current_x < patrol_left_) {
        physics_component.setVelocityX(move_speed_);
        moving_right_ = true;
    }
    sprite_component.setIsFlipped(moving_right_);
}
} // ai
} // component
} // game
