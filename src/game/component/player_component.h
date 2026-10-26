//
// Created by wangm on 26-10-24.
//

#ifndef PLAYER_COMPONENT_H
#define PLAYER_COMPONENT_H
#include "../../engine/component/component.h"

namespace game::component::state {
class PlayerState;
}

namespace engine::component {
class PhysicsComponent;
class SpriteComponent;
class TransformComponent;
class AnimationComponent;
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

    // get/set
    [[nodiscard]] engine::component::TransformComponent& getTransformComponent() const { return *transform_component_; }
    [[nodiscard]] engine::component::SpriteComponent& getSpriteComponent() const { return *sprite_component_; }
    [[nodiscard]] engine::component::PhysicsComponent& getPhysicsComponent() const { return *physics_component_; }
    [[nodiscard]] engine::component::AnimationComponent& getAnimationComponent() const { return *animation_component_; }
    [[nodiscard]] bool isIsDead() const { return is_dead_; }                   // 是否死亡
    [[nodiscard]] float getMoveForce() const { return move_force_; }           // 水平移动力
    [[nodiscard]] float getMaxSpeed() const { return max_speed_; }             // 最大移动速度
    [[nodiscard]] float getFrictionFactor() const { return friction_factor_; } // 摩擦系数
    [[nodiscard]] float getJumpForce() const { return jump_force_; }           // 跳跃力

    void setIsDead(bool is_dead) { is_dead_ = is_dead; }                                  // 设置死亡状态
    void setMoveForce(float move_force) { move_force_ = move_force; }                     // 设置水平移动力
    void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }                         // 设置最大移动速度
    void setFrictionFactor(float friction_factor) { friction_factor_ = friction_factor; } // 设置摩擦系数
    void setJumpForce(float jump_force) { jump_force_ = jump_force; }                     // 设置跳跃力

    void setState(std::unique_ptr<game::component::state::PlayerState> new_state);

protected:
    void init() override;
    void handleInput(engine::core::Context& context) override;
    void update(const float, engine::core::Context&) override;
private:
    engine::component::TransformComponent* transform_component_;
    engine::component::SpriteComponent* sprite_component_;
    engine::component::PhysicsComponent* physics_component_;
    engine::component::AnimationComponent* animation_component_;

    std::unique_ptr<game::component::state::PlayerState> current_state_; // 当前状态
    bool is_dead_{false};                                                // 是否死亡

    // 移动相关参数；
    float move_force_{200.0f};     // 水平移动力
    float max_speed_{120.0f};      // 最大移动速度
    float friction_factor_{0.85f}; // 摩擦系数
    float jump_force_{350.0f};     // 跳跃力
};
} // component
} // game

#endif //PLAYER_COMPONENT_H
