//
// Created by wangm on 26-10-21.
//

#include "physics_engine.h"

#include "../component/physics_component.h"
#include <chrono>

#include "collision.h"
#include "../component/collider_component.h"
#include "../component/transform_component.h"
#include "../object/game_object.h"

namespace engine {
namespace physics {
void PhysicsEngine::registerComponent(engine::component::PhysicsComponent& component) {
    components_.push_back(&component);
}

void PhysicsEngine::unregisterComponent(engine::component::PhysicsComponent& component) {
    auto it = std::remove(components_.begin(), components_.end(), &component);
    components_.erase(it, components_.end());
}

void PhysicsEngine::update(const std::chrono::milliseconds& dt) {
    // 清空前一帧的碰撞对；
    collision_pairs_.clear();
    // 遍历所有注册的物理组件
    for (const auto& component: components_) {
        if (!component->isEnabled()) { continue; }
        if (component->isUseGravity()) { component->addForce(gravity_ * component->getMass()); }
        float dt_s = std::chrono::duration<float>(dt).count();
        // 更新速度； v += a * dt; a = F / m;
        auto velocity = (component->getForce() / component->getMass()) * dt_s;
        component->addVelocity(velocity);

        // 更新位移；
        auto transform = component->getTransform();
        if (transform) { transform->translate(component->getVelocity() * dt_s); }
    }

    // 检查碰撞；
    checkObjectCollisions();
}

void PhysicsEngine::checkObjectCollisions() {
    for (int i = 0; i < components_.size() - 1; ++i) {
        auto& pc_a = components_[i];
        if (!pc_a->isEnabled()) { continue; }
        auto* obj_a = pc_a->getOwner();
        if (!obj_a) { continue; }
        auto* cc_a = obj_a->getComponent<engine::component::ColliderComponent>();
        if (!cc_a || !cc_a->isIsActive()) { continue; }

        for (int j = i + 1; j < components_.size(); ++j) {
            auto& pc_b = components_[j];
            if (!pc_b->isEnabled()) { continue; }
            auto* obj_b = pc_b->getOwner();
            if (!obj_b) { continue; }
            auto* cc_b = obj_b->getComponent<engine::component::ColliderComponent>();
            if (!cc_b || !cc_b->isIsActive()) { continue; }

            if (engine::physics::collision::checkCollision(*cc_a, *cc_b)) {
                // 记录碰撞
                collision_pairs_.emplace_back(obj_a, obj_b);
            }
        }
    }
}
} // physics
} // engine
