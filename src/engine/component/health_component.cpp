//
// Created by wangm on 26-10-26.
//

#include "health_component.h"

namespace engine {
namespace component {
HealthComponent::HealthComponent(int max_health, float invincibility_duration)
    : max_health_(std::max(1, max_health)),
      current_health_(max_health),
      invincibility_duration_(invincibility_duration) {}

bool HealthComponent::takeDamage(int damage_amount) {
    if (is_invincible_) { return false; }                  // 无敌状态； 不受伤害
    if (damage_amount < 1 || !isAlive()) { return false; } // 无效伤害/目标死亡； 不受伤害

    // 造成伤害；
    current_health_ -= damage_amount;
    current_health_ = std::max(0, current_health_);
    if (isAlive()) {
        setInvincible(invincibility_duration_);
    }
    return true;
}

void HealthComponent::heal(int heal_amount) {
    if (heal_amount < 0 || !isAlive()) { return; }
    setCurrentHealth(current_health_ + heal_amount);
}

void HealthComponent::setMaxHealth(int max_health) {
    max_health_ = std::max(1, max_health);
    current_health_ = std::min(current_health_, max_health_);
}

void HealthComponent::setCurrentHealth(int current_health) {
    current_health_ = std::max(0, std::min(max_health_, current_health));
}

void HealthComponent::setInvincible(float dt) {
    if (dt > 0.0) {
        is_invincible_ = true;
        invincibility_timer_ = dt;
    } else {
        is_invincible_ = false;
        invincibility_timer_ = 0.0f;
    }
}

void HealthComponent::update(const float dt, engine::core::Context& context) {
    if (!isInvincible()) { return; }
    invincibility_timer_ -= dt;
    if (invincibility_timer_ < 0.0f) {
        is_invincible_ = false;
        invincibility_timer_ = 0.0f;
    }
}
} // component
} // engine
