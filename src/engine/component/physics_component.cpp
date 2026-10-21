//
// Created by wangm on 26-10-21.
//

#include "physics_component.h"

#include <spdlog/spdlog.h>

#include "../log/log_manager.h"
#include "../log/my_log.h"
#include "../object/game_object.h"
#include "../physics/physics_engine.h"
#include "../component/transform_component.h"

namespace engine {
namespace component {
PhysicsComponent::PhysicsComponent(engine::physics::PhysicsEngine& physics_engine, bool use_gravity, float mass)
    : physics_engine_(&physics_engine), use_gravity_(use_gravity), mass_(mass) {
    mylog::MyLog{}.trace("create PhysicsComponent success");
}

void PhysicsComponent::init() {
    if (!owner_) {
        mylog::MyLog{}.error("PhysicsComponent init;owner == null");
        return;
    }
    if (!physics_engine_) {
        mylog::MyLog{}.error("PhysicsComponent init; physicsEngine == null");
        return;
    }
    transform_ = owner_->getComponent<engine::component::TransformComponent>();
    if (!transform_) {
        spdlog::warn("PhysicsComponent init;transform = null");
    }
    // 注册到PhysicsEngine;
    physics_engine_->registerComponent(*this);
    Component::init();
}

void PhysicsComponent::clear() {
    physics_engine_->unregisterComponent(*this);
    Component::clear();
}
} // component
} // engine
