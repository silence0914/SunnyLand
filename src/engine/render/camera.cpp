//
// Created by wangm on 26-10-12.
//

#include "camera.h"

#include <spdlog/spdlog.h>

#include "../component/transform_component.h"


namespace engine::render {
Camera::Camera(const glm::vec2& viewport_size, const glm::vec2& position,
               const std::optional<engine::utils::Rect>& limit_bounds)
    : viewport_size_(viewport_size), position_(position), limit_bounds_(limit_bounds) {
    clampPosition();
    spdlog::trace("create Camera success");
}

void Camera::update(const std::chrono::milliseconds& dt) {
    if (target_ == nullptr) { return; }
    glm::vec2 target_pos = target_->getPosition();
    glm::vec2 desired_position = target_pos - viewport_size_ / 2.0f; // 计算目标位置 使目标处于视口中心；
    auto distance = glm::distance(position_, desired_position);
    constexpr float SNAP_THRESHOLD = 1.0f;
    if (distance < SNAP_THRESHOLD) {
        position_ = desired_position;
    } else {
        position_ = glm::mix(position_, desired_position, smooth_speed * std::chrono::duration<float>(dt).count());
        position_ = glm::vec2(glm::round(position_.x), glm::round(position_.y)); // 四舍五入到整数；
    }
    clampPosition();
}

void Camera::move(const glm::vec2& offset) {
    position_ += offset;
    clampPosition();
}

glm::vec2 Camera::worldToScreen(const glm::vec2& world_pos) const {
    // 世界坐标减去相机坐标
    return world_pos - position_;
}

glm::vec2 Camera::worldToScreenWithParallax(const glm::vec2& world_pos, const glm::vec2& scroll_factor) const {
    // 世界坐标减去 应用滚动因子的相机坐标；
    return world_pos - position_ * scroll_factor;
}

glm::vec2 Camera::screenToWorld(const glm::vec2& screen_pos) const {
    // 屏幕坐标加上相机坐标
    return screen_pos + position_;
}

void Camera::setPosition(const glm::vec2& position) {
    position_ = position;
    clampPosition();
}

engine::component::TransformComponent* Camera::getTarget() const { return target_; }

void Camera::setLimitBounds(const std::optional<engine::utils::Rect>& limit_bounds) {
    limit_bounds_ = limit_bounds;
}

void Camera::setViewportSize(const glm::vec2& viewport_size) {
    if (viewport_size.x > 0 && viewport_size.y > 0) {
        viewport_size_ = viewport_size;
    }
}

void Camera::setTarget(engine::component::TransformComponent& target) { target_ = &target; }

void Camera::clampPosition() {
    if (!limit_bounds_.has_value()) { return; }
    if (limit_bounds_->size.x <= 0 || limit_bounds_->size.y <= 0) { return; }
    glm::vec2 min_cam_pos = limit_bounds_->position;
    glm::vec2 max_cam_pos = limit_bounds_->position + limit_bounds_->size - viewport_size_;

    min_cam_pos = glm::min(min_cam_pos, max_cam_pos);
    max_cam_pos = glm::max(min_cam_pos, max_cam_pos);

    position_ = glm::clamp(position_, min_cam_pos, max_cam_pos);
}
} // render
