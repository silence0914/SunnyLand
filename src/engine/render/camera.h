//
// Created by wangm on 26-10-12.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <optional>

#include "../component/component.h"
#include "../utils/math.h"

namespace engine::component {
class TransformComponent;
}

namespace engine::render {
/**
 * @brief 相机控制
 */
class Camera final {
public:
    explicit Camera(const glm::vec2& viewport_size, const glm::vec2& position = glm::vec2{0.0f, 0.0f},
                    const std::optional<engine::utils::Rect>& limit_bounds = std::nullopt);

    void update(const std::chrono::milliseconds& dt); // 更新相机位置；
    void move(const glm::vec2& offset);               // 移动相机位置;

    [[nodiscard]] glm::vec2 worldToScreen(const glm::vec2& world_pos) const; // 世界坐标转屏幕坐标；
    /**
     * @brief 视差滚动的世界坐标转屏幕坐标
     * @param world_pos  世界坐标
     * @param scroll_factor 滚动因子；
     * @return
     */
    [[nodiscard]] glm::vec2 worldToScreenWithParallax(const glm::vec2& world_pos, const glm::vec2& scroll_factor) const;
    [[nodiscard]] glm::vec2 screenToWorld(const glm::vec2& screen_pos) const; // 屏幕坐标转世界坐标；

    [[nodiscard]] glm::vec2 getPosition1() const { return position_; }                                 // 获取相机坐标；
    [[nodiscard]] std::optional<engine::utils::Rect> getLimitBounds1() const { return limit_bounds_; } // 获取限制相机移动范围；
    [[nodiscard]] glm::vec2 getViewportSize() const { return viewport_size_; }                         // 获取视窗大小
    [[nodiscard]] engine::component::TransformComponent* getTarget() const;                            // 获取跟随目标变换的组件；


    void setLimitBounds(const std::optional<engine::utils::Rect>& limit_bounds); // 设置相机移动范围；
    void setPosition(const glm::vec2& position);                                 // 设置相机坐标
    void setViewportSize(const glm::vec2& viewport_size);                        // 设置视窗大小；
    void setTarget(engine::component::TransformComponent& target);               // 设置跟随目标变换的组件；


    Camera(const Camera& other) = delete;
    Camera(Camera&& other) noexcept = delete;
    Camera& operator=(const Camera& other) = delete;
    Camera& operator=(Camera&& other) noexcept = delete;

private:
    glm::vec2 viewport_size_;                         // 视窗大小
    glm::vec2 position_;                              // 相机坐标
    std::optional<engine::utils::Rect> limit_bounds_; // 相机移动范围， nullopt 表示不限制；
    float smooth_speed = 5.0f;                        // 相机移动的平滑速度；
    engine::component::TransformComponent* target_{nullptr};

private:
    void clampPosition(); // 限制相机位置在边界内；
};
} // render
// engine

#endif //CAMERA_H
