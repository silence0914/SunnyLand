//
// Created by wangm on 26-10-27.
//

#include "ai_component.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/log/my_log.h"

namespace game {
namespace component {
void AiComponent::setBehavior(std::unique_ptr<ai::AiBehavior> behavior) {
    current_behavior_ = std::move(behavior);
    if (current_behavior_) {
        current_behavior_->enter(*this);
    }
}

bool AiComponent::takeDamage(int damage) {
    if (auto health_component = owner_->getComponent<engine::component::HealthComponent>()) {
        return health_component->takeDamage(damage);
    }
    return false;
}

bool AiComponent::isAlive() {
    if (auto health_component = getOwner()->getComponent<engine::component::HealthComponent>()) {
        return health_component->isAlive();
    }
    return true;
}

void AiComponent::init() {
    if (!owner_) {
        mylog::MyLog{}.error("owner is null");
        return;
    }

    transform_component_ = owner_->getComponent<engine::component::TransformComponent>();
    physics_component_ = owner_->getComponent<engine::component::PhysicsComponent>();
    sprite_component_ = owner_->getComponent<engine::component::SpriteComponent>();
    animation_component_ = owner_->getComponent<engine::component::AnimationComponent>();

    if (!transform_component_ || !physics_component_ || !sprite_component_ || !animation_component_) {
        mylog::MyLog{}.
                error("owner has no transform_component or physics_component or sprite_component or animation_component");
    }
}

void AiComponent::update(const float dt, engine::core::Context& context) {
    if (!current_behavior_) { return; }
    if (current_behavior_) {
        current_behavior_->update(dt, *this);
    }
}
} // component
} // game
