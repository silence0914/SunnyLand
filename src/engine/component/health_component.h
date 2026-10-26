//
// Created by wangm on 26-10-26.
//

#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H
#include "component.h"

namespace engine {
namespace component {
class HealthComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    HealthComponent(int max_health = 1, float invincibility_duration = 2.0f);

    ~HealthComponent() override = default;

    HealthComponent(const HealthComponent& other) = delete;
    HealthComponent(HealthComponent&& other) noexcept = delete;
    HealthComponent& operator=(const HealthComponent& other) = delete;
    HealthComponent& operator=(HealthComponent&& other) noexcept = delete;

    bool takeDamage(int damage_amount); // 伤害；
    void heal(int heal_amount);         // 治疗

    // get/set
    [[nodiscard]] int getMaxHealth() const { return max_health_; }         // 最大血量
    [[nodiscard]] int getCurrentHealth() const { return current_health_; } // 当前血量
    [[nodiscard]] bool isInvincible() const { return is_invincible_; }     // 是否处于无敌状态；
    [[nodiscard]] bool isAlive() const { return current_health_ > 0; }     // 是否存活；

    void setMaxHealth(int max_health);
    void setCurrentHealth(int current_health);
    void setInvincible(float dt); // 进入无敌时间
    void setInvincibilityDuration(float invincibility_duration) { invincibility_duration_ = invincibility_duration; }

protected:
    void update(const float, engine::core::Context&) override;

private:
    int max_health_{1};                  // 最大血量
    int current_health_{1};              // 当前血量
    bool is_invincible_{false};          // 是否处于无敌状态；
    float invincibility_duration_{2.0f}; // 受伤后的无敌时间；
    float invincibility_timer_{0.0f};    //无敌时间计时器；
};
} // component
} // engine

#endif //HEALTH_COMPONENT_H
