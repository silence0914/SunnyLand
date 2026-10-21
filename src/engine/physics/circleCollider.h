//
// Created by wangm on 26-10-21.
//

#ifndef CIRCLECOLLIDER_H
#define CIRCLECOLLIDER_H
#include <glm/vec2.hpp>

#include "collider.h"

namespace engine {
namespace physics {
/**
 * @brief 圆形碰撞
 */
class CircleCollider final : engine::physics::Collider {
public:
    /**
     * @brief 构造函数
     * @param radius 碰撞盒半径
     */
    explicit CircleCollider(float radius) : radius_(radius) { setAabbSize(glm::vec2(radius * 2.0f, radius * 2.0f)); };
    ~CircleCollider() override = default;

    // get/set
    engine::physics::ColliderType getType() const override { return engine::physics::ColliderType::CIRCLE; }
    [[nodiscard]] float getRadius() const { return radius_; }
    void setRadius(float radius) { radius_ = radius; }

private:
    float radius_ = 0.0f; // 圆半径
};
} // physics
} // engine

#endif //CIRCLECOLLIDER_H
