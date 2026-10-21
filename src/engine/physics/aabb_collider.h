//
// Created by wangm on 26-10-21.
//

#ifndef AABB_COLLIDER_H
#define AABB_COLLIDER_H
#include <glm/vec2.hpp>
#include "collider.h"

namespace engine {
namespace physics {
/**
 * @brief aabb碰撞盒
 */
class AABBCollider final : public engine::physics::Collider {
public:
    /**
     * @brief
     * @param size 矩形碰撞盒的尺寸
     */
    explicit AABBCollider(const glm::vec2& size) : size_(size) { setAabbSize(size); }
    ~AABBCollider() override = default;

    // get/set
    ColliderType getType() const override { return engine::physics::ColliderType::AABB; };
    [[nodiscard]] glm::vec2 getSize() const { return size_; }
    void setSize(const glm::vec2& size) { size_ = size; }

private:
    glm::vec2 size_ = {0.0f, 0.0f}; // 包围盒的尺寸
};
} // physics
} // engine

#endif //AABB_COLLIDER_H
