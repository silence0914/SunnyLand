//
// Created by wangm on 26-10-14.
//

#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H
#include <glm/glm.hpp>

#include "component.h"

namespace engine {
namespace component {
/**
 * @brief 管理 GameObject 的位置，旋转，缩放
 */
class TransformComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    TransformComponent(const glm::vec2& position = {0, 0}, const glm::vec2& scale = {1, 1}, float rotation = 0.0f)
        : position_(position), scale_(scale), rotation_(rotation) {}

    TransformComponent(const TransformComponent& other) = delete;
    TransformComponent(TransformComponent&& other) noexcept = delete;
    TransformComponent& operator=(const TransformComponent& other) = delete;
    TransformComponent& operator=(TransformComponent&& other) noexcept = delete;

    // get/set
    [[nodiscard]] glm::vec2 getPosition() const { return position_; }
    void setPosition(const glm::vec2& position) { position_ = position; }

    [[nodiscard]] glm::vec2 getScale() const { return scale_; }
    void setScale(const glm::vec2& scale); // 设置缩放比例同步更像偏移量；

    [[nodiscard]] float getRotation() const { return rotation_; }
    void setRotation(float rotation) { rotation_ = rotation; }

    void translate(const glm::vec2& offset) { position_ += offset; }

private:
    glm::vec2 position_{}; // 位置
    glm::vec2 scale_{};    // 缩放
    float rotation_{0.0f}; // 旋转

private:
    void update(const std::chrono::milliseconds&, engine::core::Context&) override {};
};
} // component
} // engine

#endif //TRANSFORM_COMPONENT_H
