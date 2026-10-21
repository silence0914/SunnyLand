//
// Created by wangm on 26-10-21.
//

#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H
#include <glm/vec2.hpp>

#include "../component/component.h"

namespace engine::component {
class PhysicsComponent;
}

namespace engine {
namespace physics {
/**
 * @brief 负责 管理和模拟 物理行为和碰撞检测
 */
class PhysicsEngine {
public:
    PhysicsEngine() = default;
    ~PhysicsEngine() = default;

    PhysicsEngine(const PhysicsEngine& other) = delete;
    PhysicsEngine(PhysicsEngine&& other) noexcept = delete;
    PhysicsEngine& operator=(const PhysicsEngine& other) = delete;
    PhysicsEngine& operator=(PhysicsEngine&& other) noexcept = delete;

    void registerComponent(engine::component::PhysicsComponent& component);   // 注册物理组件
    void unregisterComponent(engine::component::PhysicsComponent& component); // 注销物理组件

    void update(const std::chrono::milliseconds& dt); // 核心循环， 更新所有注册的物理组件

    // get/set
    [[nodiscard]] glm::vec2 getGravity() const { return gravity_; } // 获取当前全局重力
    [[nodiscard]] float getMaxSpeed() const { return max_speed_; }  // 获取当前最大速度

    void setGravity(const glm::vec2& gravity) { gravity_ = gravity; } // 设置全局重力
    void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }     // 设置最大速度

private:
    std::vector<engine::component::PhysicsComponent*> components_; // 注册的物理组件容器 非拥有指针；
    glm::vec2 gravity_ = {0.0f, 980.0f};                           // 默认重力， 像素/秒^2
    float max_speed_ = 500.0f;                                     // 最大速度 像素/秒；
};
} // physics
} // engine

#endif //PHYSICS_ENGINE_H
