//
// Created by wangm on 26-10-21.
//

#ifndef PHYSICS_COMPENENT_H
#define PHYSICS_COMPENENT_H
#include <glm/common.hpp>
#include <glm/vec2.hpp>

#include "component.h"
#include "../physics/physics_engine.h"

namespace engine::component {
class TransformComponent;
}

namespace engine::physics {
class PhysicsEngine;
}

namespace engine {
namespace component {
/**
 * @brief 管理 GameObject 的物理属性
 *
 * 储存速度，质量，力和重力设置， 与PhysicsEngine交互
 */
class PhysicsComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    /**
     * @brief
     * @param physics_engine PhysicsEngine 对象；
     * @param use_gravity 该对象是否重力影响 默认true
     * @param mass 该对象的质量 默认1.0；
     */
    PhysicsComponent(engine::physics::PhysicsEngine& physics_engine, bool use_gravity = true, float mass = 1.0f);
    ~PhysicsComponent() override = default;

    PhysicsComponent(const PhysicsComponent& other) = delete;
    PhysicsComponent(PhysicsComponent&& other) noexcept = delete;
    PhysicsComponent& operator=(const PhysicsComponent& other) = delete;
    PhysicsComponent& operator=(PhysicsComponent&& other) noexcept = delete;

    // 物理方法；
    void addForce(const glm::vec2& force) { force_ += force; } // 添加力
    void clearForce() { force_ = {0, 0}; }                     // 清除力
    void addVelocity(const glm::vec2& velocity) {
        velocity_ += velocity;
        clampVelocity();
    }

    [[nodiscard]] glm::vec2 getVelocity() const { return velocity_; } // 获取速度
    [[nodiscard]] float getMass() const { return mass_; }             // 获取质量
    [[nodiscard]] bool isUseGravity() const { return use_gravity_; }  // 是否守重力影响
    [[nodiscard]] bool isEnabled() const { return enabled_; }         // 是否激活
    [[nodiscard]] glm::vec2 getForce() const { return force_; }
    [[nodiscard]] engine::component::TransformComponent* getTransform() const { return transform_; }

    void setVelocity(const glm::vec2& velocity) {
        velocity_ = velocity;
        clampVelocity();
    }                                                                    // 设置速度
    void setMass(float mass) { mass_ = mass; }                           // 设置质量
    void setUseGravity(bool use_gravity) { use_gravity_ = use_gravity; } // 设置是否受重力影响
    void setEnabled(bool enabled) { enabled_ = enabled; }                // 设置是否激活

private:
    void init() override;
    void update(const std::chrono::milliseconds&, engine::core::Context&) override {};
    void clear() override;

    void clampVelocity() {
        velocity_ = glm::clamp(velocity_, -physics_engine_->getMaxSpeed(), physics_engine_->getMaxSpeed());
    }

private:
    engine::physics::PhysicsEngine* physics_engine_{nullptr};
    engine::component::TransformComponent* transform_{nullptr};

    glm::vec2 velocity_{0.0f, 0.0f}; // 物体速度；
    glm::vec2 force_{0.0f, 0.0f};    // 当前受到的力
    float mass_{1.0f};               // 物体的质量
    bool use_gravity_{true};         //是否受重力影响
    bool enabled_{true};             //是否激活
};
} // component
} // engine

#endif //PHYSICS_COMPENENT_H
