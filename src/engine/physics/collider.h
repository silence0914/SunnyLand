//
// Created by wangm on 26-10-21.
//

#ifndef COLLIDER_H
#define COLLIDER_H

namespace engine {
namespace physics {
enum class ColliderType {
    NONE,
    AABB,
    CIRCLE,
};

/**
 * @brief 碰撞器的抽象类型
 * 具有碰撞模型的对象 继承
 */
class Collider {
public:
    Collider() = default;
    virtual ~Collider() = default;

    virtual ColliderType getType() const = 0;
    [[nodiscard]] glm::vec2 getAABBSize() const { return aabb_size_; }
    void setAabbSize(const glm::vec2& aabb_size) { aabb_size_ = aabb_size; }

protected:
    glm::vec2 aabb_size_ = {0.0f, 0.0f}; // 最小包围盒的尺寸，
};
} // physics
} // engine

#endif //COLLIDER_H
