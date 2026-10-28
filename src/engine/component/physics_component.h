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
    explicit PhysicsComponent(engine::physics::PhysicsEngine& physics_engine, bool use_gravity = true,
                              float mass = 1.0f);
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

    void setVelocity(const glm::vec2& velocity) {
        velocity_ = velocity;
        clampVelocity();
    } // 设置速度

    void setVelocityX(float vx) {
        velocity_.x = vx;
        clampVelocity();
    } // 设置x方向速度

    void setVelocityY(float vy) {
        velocity_.y = vy;
        clampVelocity();
    } // 设置y方向速度


    // get/set
    [[nodiscard]] glm::vec2 getVelocity() const { return velocity_; } // 获取速度
    [[nodiscard]] float getMass() const { return mass_; }             // 获取质量
    [[nodiscard]] bool isUseGravity() const { return use_gravity_; }  // 是否守重力影响
    [[nodiscard]] bool isEnabled() const { return enabled_; }         // 是否激活
    [[nodiscard]] glm::vec2 getForce() const { return force_; }
    [[nodiscard]] engine::component::TransformComponent* getTransform() const { return transform_; }
    [[nodiscard]] bool isCollidedBelow() const { return collided_below_; }
    [[nodiscard]] bool isCollidedAbove() const { return collided_above_; }
    [[nodiscard]] bool isCollidedLeft() const { return collided_left_; }
    [[nodiscard]] bool isCollidedRight() const { return collided_right_; }
    [[nodiscard]] bool isCollidedLadder() const { return collided_ladder; }
    [[nodiscard]] bool isOnTopLadder() const { return is_on_top_ladder; }

    void setMass(float mass) { mass_ = mass; }                                                  // 设置质量
    void setUseGravity(bool use_gravity) { use_gravity_ = use_gravity; }                        // 设置是否受重力影响
    void setEnabled(bool enabled) { enabled_ = enabled; }                                       // 设置是否激活
    void setCollidedBelow(bool collided_below) { collided_below_ = collided_below; }            // 设置下方是否碰撞；
    void setCollidedAbove(bool collided_above) { collided_above_ = collided_above; }            // 设置上方是否碰撞；
    void setCollidedLeft(bool collided_left) { collided_left_ = collided_left; }                // 设置左侧是否碰撞；
    void setCollidedRight(bool collided_right) { collided_right_ = collided_right; }            // 设置右侧是否碰撞；
    void setCollidedLadder(bool collided_ladder) { this->collided_ladder = collided_ladder; }   // 设置是否与梯子碰撞；
    void setIsOnTopLadder(bool is_on_top_ladder) { this->is_on_top_ladder = is_on_top_ladder; } // 设置是否在梯子顶部；

    void resetCollisionFlags() {
        collided_above_ = false;
        collided_below_ = false;
        collided_left_ = false;
        collided_right_ = false;
        collided_ladder = false;
    }

private:
    void init() override;
    void update(const float, engine::core::Context&) override {};
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

    // 碰撞标志；
    bool collided_below_{false};
    bool collided_above_{false};
    bool collided_left_{false};
    bool collided_right_{false};
    bool collided_ladder{false};
    bool is_on_top_ladder{false};
};
} // component
} // engine

#endif //PHYSICS_COMPENENT_H
