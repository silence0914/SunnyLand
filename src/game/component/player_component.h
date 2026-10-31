//
// Created by wangm on 26-10-24.
//

#ifndef PLAYER_COMPONENT_H
#define PLAYER_COMPONENT_H
#include "../../engine/component/component.h"

namespace engine::component {
class HealthComponent;
}

namespace game::component::state {
class PlayerState;
}

namespace engine::component {
class PhysicsComponent;
class SpriteComponent;
class TransformComponent;
class AnimationComponent;
class AudioComponent;
}

namespace game {
namespace component {
class PlayerComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    PlayerComponent() = default;

    ~PlayerComponent() override = default;

    PlayerComponent(const PlayerComponent& other) = delete;
    PlayerComponent(PlayerComponent&& other) noexcept = delete;
    PlayerComponent& operator=(const PlayerComponent& other) = delete;
    PlayerComponent& operator=(PlayerComponent&& other) noexcept = delete;

    bool takeDamage(int damage); // 试图造成伤害； 成功返回true;

    // get/set
    [[nodiscard]] engine::component::TransformComponent& getTransformComponent() const { return *transform_component_; }
    [[nodiscard]] engine::component::SpriteComponent& getSpriteComponent() const { return *sprite_component_; }
    [[nodiscard]] engine::component::PhysicsComponent& getPhysicsComponent() const { return *physics_component_; }
    [[nodiscard]] engine::component::AnimationComponent& getAnimationComponent() const { return *animation_component_; }
    [[nodiscard]] engine::component::HealthComponent& getHealthComponent() const { return *health_component_; }
    [[nodiscard]] engine::component::AudioComponent& getAudioComponent() const { return *audio_component_; }

    [[nodiscard]] bool isDead() const { return is_dead_; }                     // 是否死亡
    [[nodiscard]] float getMoveForce() const { return move_force_; }             // 水平移动力
    [[nodiscard]] float getMaxSpeed() const { return max_speed_; }               // 最大移动速度
    [[nodiscard]] float getFrictionFactor() const { return friction_factor_; }   // 摩擦系数
    [[nodiscard]] float getJumpVel() const { return jump_vel_; }                 // 跳跃速度
    [[nodiscard]] float getStunnedDuration() const { return stunned_duration_; } // 硬直时间；
    [[nodiscard]] float getClimbSpeed() const { return climb_speed_; }           // 爬梯速度

    void setIsDead(bool is_dead) { is_dead_ = is_dead; }                                      // 设置死亡状态
    void setMoveForce(float move_force) { move_force_ = move_force; }                         // 设置水平移动力
    void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }                             // 设置最大移动速度
    void setFrictionFactor(float friction_factor) { friction_factor_ = friction_factor; }     // 设置摩擦系数
    void setJumpVel(float jump_vel) { jump_vel_ = jump_vel; }                                 // 设置跳跃力
    void setStunnedDuration(float stunned_duration) { stunned_duration_ = stunned_duration; } // 设置硬直时间；
    void setClimbSpeed(float climb_speed) { climb_speed_ = climb_speed; }                     // 设置爬梯速度

    void setState(std::unique_ptr<game::component::state::PlayerState> new_state);
    [[nodiscard]] bool is_on_ground() const;

protected:
    void init() override;
    void handleInput(engine::core::Context& context) override;
    void update(const float, engine::core::Context&) override;

private:
    engine::component::TransformComponent* transform_component_;
    engine::component::SpriteComponent* sprite_component_;
    engine::component::PhysicsComponent* physics_component_;
    engine::component::AnimationComponent* animation_component_;
    engine::component::HealthComponent* health_component_;
    engine::component::AudioComponent* audio_component_;

    std::unique_ptr<game::component::state::PlayerState> current_state_; // 当前状态
    bool is_dead_{false};                                                // 是否死亡

    // 移动相关参数；
    float move_force_{200.0f};     // 水平移动力
    float max_speed_{120.0f};      // 最大移动速度
    float friction_factor_{0.85f}; // 摩擦系数
    float jump_vel_{350.0f};       // 跳跃速度
    float climb_speed_{100.0f};    // 爬梯速度
    float stunned_duration_{0.4f}; // 硬直时间；

    // 无敌时间闪烁
    static constexpr float flash_interval_ = 0.1f; // 闪烁间隔时间
    float flash_timer_ = 0.0f;                     // 闪烁计时器；用于无敌状态下的闪烁效果；

    // 土狼时间
    static constexpr float coyote_time_ = 0.1f; // Coyote Time 持续时长
    float coyote_timer_ = 0.0f;                 // Coyote Time 计时器
};
} // component
} // game

#endif //PLAYER_COMPONENT_H
