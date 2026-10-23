//
// Created by wangm on 26-10-21.
//

#include "collider_component.h"

#include <spdlog/spdlog.h>

#include "transform_component.h"
#include "../log/log_manager.h"
#include "../log/my_log.h"
#include "../object/game_object.h"
#include "../utils/math.h"

namespace engine {
namespace component {
ColliderComponent::ColliderComponent(std::unique_ptr<engine::physics::Collider> collider,
                                     engine::utils::Alignment alignment, bool is_trigger, bool is_active)
    : collider_(std::move(collider)), alignment_(alignment), is_trigger_(is_trigger), is_active_(is_active) {
    if (!collider_) {
        mylog::MyLog{}.error("ColliderComponent: collider is null");
    }
}

void ColliderComponent::updateOffset() {
    if (!collider_) { return; }

    // 获取碰撞盒 最小包围盒尺寸
    auto collider_size = collider_->getAABBSize();
    if (collider_size.x <= 0.0f || collider_size.y <= 0.0f) {
        offset_ = {0.0f, 0.0f};
        return;
    }
    auto scale = transform_->getScale();

    switch (alignment_) {
        case engine::utils::Alignment::TOP_LEFT:
            offset_ = glm::vec2{0.0f, 0.0f} * scale;
            break;
        case engine::utils::Alignment::TOP_CENTER:
            offset_ = glm::vec2{-collider_size.x / 2.0f, 0.0f} * scale;
            break;
        case engine::utils::Alignment::TOP_RIGHT:
            offset_ = glm::vec2{-collider_size.x, 0.0f} * scale;
            break;
        case engine::utils::Alignment::CENTER_LEFT:
            offset_ = glm::vec2{0.0f, -collider_size.y / 2.0f} * scale;
            break;
        case engine::utils::Alignment::CENTER:
            offset_ = glm::vec2{-collider_size.x / 2.0f, -collider_size.y / 2.0f} * scale;
            break;
        case engine::utils::Alignment::CENTER_RIGHT:
            offset_ = glm::vec2{-collider_size.x, -collider_size.y / 2.0f} * scale;
            break;
        case engine::utils::Alignment::BOTTOM_LEFT:
            offset_ = glm::vec2{0.0f, -collider_size.y} * scale;
            break;
        case engine::utils::Alignment::BOTTOM_CENTER:
            offset_ = glm::vec2{-collider_size.x / 2.0f, -collider_size.y} * scale;
            break;
        case engine::utils::Alignment::BOTTOM_RIGHT:
            offset_ = glm::vec2{-collider_size.x, -collider_size.y} * scale;
            break;
        default:
            break; // 如果 alignment_ 是 NONE，则不做任何操作（手动设置 offset_）
    }
}

engine::utils::Rect ColliderComponent::getWorldAABB() const {
    if (!transform_ || !collider_) {
        return {glm::vec2(0.0f, 0.0f)};
    }
    const glm::vec2 top_left_pos = transform_->getPosition() + offset_;
    const glm::vec2 base_size = collider_->getAABBSize();
    const glm::vec2 scale = transform_->getScale();
    glm::vec2 scale_size = base_size * scale;
    return engine::utils::Rect{top_left_pos, scale_size};
}

void ColliderComponent::init() {
    if (!owner_) {
        is_active_ = false;
        return;
    }
    transform_ = owner_->getComponent<TransformComponent>();
    if (!transform_) {
        is_active_ = false;
        return;
    }

    updateOffset();
    Component::init();
}
} // component
} // engine
