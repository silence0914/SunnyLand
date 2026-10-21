//
// Created by wangm on 26-10-21.
//

#include "physics_engine.h"

#include "../component/physics_component.h"
#include <chrono>

#include "../component/transform_component.h"

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
    // 遍历所有注册的物理组件
    for (const auto& component: components_) {
        if (!component->isEnabled()) { continue; }
        if (component->isUseGravity()) { component->addForce(gravity_ * component->getMass()); }
        float dt_s = std::chrono::duration<float>(dt).count();
        // 更新速度； v += a * dt; a = F / m;
        auto velocity = (component->getForce() / component->getMass()) * dt_s;
        component->addVelocity( velocity);

        // 更新位移；
        auto transform = component->getTransform();
        if (transform) { transform->translate(component->getVelocity() * dt_s); }
    }
}
} // physics
} // engine
