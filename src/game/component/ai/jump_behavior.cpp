//
// Created by wangm on 26-10-27.
//

#include "jump_behavior.h"

#include "../ai_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/audio_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/log/my_log.h"

namespace game {
namespace component {
namespace ai {
JumpBehavior::JumpBehavior(float min_x, float max_x, const glm::vec2& jump_vel,
                           float jump_interval): patrol_min_x_(min_x),
                                                 patrol_max_x_(max_x),
                                                 jump_vel_(jump_vel),
                                                 jump_interval_(jump_interval) {
    if (patrol_min_x_ >= patrol_max_x_) {
        mylog::MyLog{}.error("JumpBehavior: patrol_min_x_ >= patrol_max_x_");
        patrol_min_x_ = patrol_max_x_;
    }
    if (jump_interval_ <= 0.0f) {
        mylog::MyLog{}.error("JumpBehavior: jump_interval <= 0.0f");
        jump_interval_ = 2.0f;
    }
    jump_vel_.x = std::abs(jump_vel_.x);
    if (jump_vel_.y > 0) {
        mylog::MyLog{}.error("JumpBehavior: jump_vel.y > 0; jump_vel.y = {}", jump_vel_.y);
        jump_vel_.y = -300.0f;
    }
}

void JumpBehavior::enter(game::component::AiComponent& ai_component) {
    AiBehavior::enter(ai_component);
}

void JumpBehavior::update(const float dt, game::component::AiComponent& ai_component) {
    auto& animation_component = ai_component.getAnimationComponent();
    auto& physics_component = ai_component.getPhysicsComponent();
    auto& sprite_component = ai_component.getSpriteComponent();
    auto& transform_component = ai_component.getTransformComponent();
    auto& audio_component = ai_component.getAudioComponent();

    auto is_on_ground = physics_component.isCollidedBelow();
    if (!is_on_ground) {
        // 在空中，  播放对应动画；
        if (physics_component.getVelocity().y > 0) {
            animation_component.playAnimation(toString(State::FALL));
        } else {
            animation_component.playAnimation(toString(State::JUMP));
        }
        return;
    }

    audio_component.playSound("cry", -1, true);

    jump_timer_ += dt;
    physics_component.setVelocityX(0.0f);

    // 到时间， 准备跳跃
    if (jump_timer_ > jump_interval_) {
        jump_timer_ = 0.0f; //重置计时器；

        // 检查跳跃方向
        auto current_x = transform_component.getPosition().x;
        if (jumping_right_ && (physics_component.isCollidedRight() || current_x > patrol_max_x_)) {
            jumping_right_ = false;
        } else if (!jumping_right_ && (physics_component.isCollidedLeft() || current_x < patrol_min_x_)) {
            jumping_right_ = true;
        }
        auto jump_vel_x = jumping_right_ ? jump_vel_.x : -jump_vel_.x;
        physics_component.setVelocity({jump_vel_x, jump_vel_.y});
        animation_component.playAnimation(toString(State::JUMP));
        sprite_component.setIsFlipped(jumping_right_);
    } else {
        animation_component.playAnimation(toString(State::IDLE));
    }
}
} // ai
} // component
} // game
