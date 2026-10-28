//
// Created by wangm on 26-10-27.
//

#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H
#include "../../engine/object/game_object.h"
#include "ai/jump_behavior.h"

namespace engine::component {
class AnimationComponent;
class SpriteComponent;
class PhysicsComponent;
class TransformComponent;
}

namespace game {
namespace component {
/**
 * @brief 管理 GameObject 的ai行为；
 *
 * 使用策略模式， 持有一个AiBehavior实例来执行ai逻辑
 */
class AiComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    AiComponent() = default;
    ~AiComponent() override = default;


    void setBehavior(std::unique_ptr<ai::AiBehavior> behavior); // 设置ai行为
    bool takeDamage(int damage);                                // 伤害判定
    bool isAlive();                                             // 检查对象是否存活

    // get
    [[nodiscard]] engine::component::TransformComponent& getTransformComponent() const { return *transform_component_; }
    [[nodiscard]] engine::component::AnimationComponent& getAnimationComponent() const { return *animation_component_; }
    [[nodiscard]] engine::component::PhysicsComponent& getPhysicsComponent() const { return *physics_component_; }
    [[nodiscard]] engine::component::SpriteComponent& getSpriteComponent() const { return *sprite_component_; }

    AiComponent(const AiComponent& other) = delete;
    AiComponent(AiComponent&& other) noexcept = delete;
    AiComponent& operator=(const AiComponent& other) = delete;
    AiComponent& operator=(AiComponent&& other) noexcept = delete;

protected:
    void init() override;
    void update(const float, engine::core::Context&) override;

private:
    engine::component::TransformComponent* transform_component_;
    engine::component::AnimationComponent* animation_component_;
    engine::component::PhysicsComponent* physics_component_;
    engine::component::SpriteComponent* sprite_component_;

    std::unique_ptr<game::component::ai::AiBehavior> current_behavior_{nullptr};
};
} // component
} // game

#endif //AI_COMPONENT_H
