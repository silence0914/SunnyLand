//
// Created by wangm on 26-10-21.
//

#include "collision.h"
#include "../component/collider_component.h"
#include "../component/transform_component.h"

namespace engine {
namespace physics {
namespace collision {
bool checkCollision(const engine::component::ColliderComponent& a, const engine::component::ColliderComponent& b) {
    auto a_collider = a.getCollider();
    auto b_collider = b.getCollider();
    auto a_transform = a.getTransform();
    auto b_transform = b.getTransform();

    auto a_size = a_collider->getAabbSize();
    auto b_size = b_collider->getAabbSize();
    auto a_pos = a_transform->getPosition();
    auto b_pos = b_transform->getPosition();

    // 如果最小包围盒未碰撞， 直接饭返回；
    if (!checkAABBOverlap(a_pos, a_size, b_pos, b_size)) {
        return false;
    }

    auto a_type = a_collider->getType();
    auto b_type = b_collider->getType();

    if (a_type == ColliderType::AABB && b_type == ColliderType::AABB) {
        return true;
    } else if (a_type == ColliderType::CIRCLE && b_type == ColliderType::CIRCLE) {
        return checkCircleOverlap(a_pos + 0.5f * a_size, 0.5f * a_size.x,
                                  b_pos + 0.5f * b_size, 0.5f * b_size.x);
    } else if (a_type == ColliderType::AABB && b_type == ColliderType::CIRCLE) {
        auto b_center = b_pos + 0.5f * b_size;
        auto b_radius = 0.5f * b_size.x;
        auto nearest_point = glm::clamp(b_center, a_pos, a_pos + a_size); // 计算圆心到AABB的最邻近点
        return checkPointInCircle(nearest_point, b_center, b_radius);
    } else if (a_type == ColliderType::CIRCLE && b_type == ColliderType::AABB) {
        auto a_center = a_pos + 0.5f * a_size;
        auto a_radius = 0.5f * a_size.x;
        auto nearest_point = glm::clamp(a_center, b_pos, b_pos + b_size); // 计算圆心到AABB的最邻近点
        return checkPointInCircle(nearest_point, a_center, a_radius);
    }
}

bool checkCircleOverlap(const glm::vec2& a_center_pos, const float a_radius, const glm::vec2& b_center_pos,
                        const float b_radius) {
    return (glm::length(a_center_pos - b_center_pos) < a_radius + b_radius);
}

bool checkAABBOverlap(const glm::vec2& a_pos, const glm::vec2& a_size, const glm::vec2& b_pos,
                      const glm::vec2& b_size) {
    if (a_pos.x + a_size.x < b_pos.x ||
        b_pos.x + b_size.x < a_pos.x ||
        a_pos.y + a_size.y < b_pos.y ||
        b_pos.y + b_size.y < a_pos.y) {
        return false;
    }
    return true;
}

bool checkRectOverlap(const engine::utils::Rect& a, const engine::utils::Rect& b) {
    return checkAABBOverlap(a.position, a.size, b.position, b.size);
}

bool checkPointInCircle(const glm::vec2& point, const glm::vec2& center, const float radius) {
    return (glm::length(point - center) < radius);
}
}
} // physics
} // engine
