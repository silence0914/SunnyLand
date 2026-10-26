//
// Created by wangm on 26-10-21.
//

#ifndef COLLIDER_COMPONENT_H
#define COLLIDER_COMPONENT_H
#include "component.h"
#include "physics_component.h"
#include "../physics/collider.h"
#include "../utils/action.h"
#include "../utils/alignment.h"
#include "../utils/math.h"


namespace engine::component {
class TransformComponent;
}

namespace engine {
namespace component {
class ColliderComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    ColliderComponent(std::unique_ptr<engine::physics::Collider> collider,
                      engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
                      bool is_trigger = false,
                      bool is_active = true
    );
    ~ColliderComponent() = default;

    // 根据当前的 alignment 和 collider 尺寸计算 offset
    void updateOffset();

    // get/set
    [[nodiscard]] const engine::component::TransformComponent* getTransform() const { return transform_; }
    [[nodiscard]] const engine::physics::Collider* getCollider() const { return collider_.get(); }
    [[nodiscard]] glm::vec2 getOffset() const { return offset_; }                      // 碰撞盒偏移量；
    [[nodiscard]] engine::utils::Alignment getAlignment() const { return alignment_; } // 碰撞盒对齐方式
    [[nodiscard]] engine::utils::Rect getWorldAABB() const;                            // 获取世界坐标下的最小包围 碰撞盒
    [[nodiscard]] bool isTrigger() const { return is_trigger_; }                     // 是否为触发器
    [[nodiscard]] bool isActive() const { return is_active_; }                       // 是否激活

    void setOffset(const glm::vec2& offset) { offset_ = offset; }                     // 设置碰撞盒偏移量；
    void setAlignment(engine::utils::Alignment alignment) { alignment_ = alignment; } // 设置碰撞盒对齐方式
    void setIsTrigger(bool is_trigger) { is_trigger_ = is_trigger; }                  // 设置是否为触发器
    void setIsActive(bool is_active) { is_active_ = is_active; }                      // 设置是否激活

private:
    engine::component::TransformComponent* transform_{nullptr};
    std::unique_ptr<engine::physics::Collider> collider_;        // 持有 碰撞器对象
    glm::vec2 offset_{0.0f, 0.0f};                               // 碰撞盒偏移量；
    engine::utils::Alignment alignment_{utils::Alignment::NONE}; // 碰撞盒 对齐方式；
    bool is_trigger_{false};                                     // 是否为触发器（仅检测碰撞，不产生物理响应）
    bool is_active_{true};                                       // 是否激活

private:
    void init() override;
    void update(const float, engine::core::Context&) override {};
};
} // component
} // engine

#endif //COLLIDER_COMPONENT_H
