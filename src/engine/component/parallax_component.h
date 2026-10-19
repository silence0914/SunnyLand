//
// Created by wangm on 26-10-19.
//

#ifndef PARALLAX_COMPONENT_H
#define PARALLAX_COMPONENT_H

#include <glm/vec2.hpp>

#include "component.h"
#include "../render/sprite.h"

namespace engine {
namespace component {
class TransformComponent;

/**
 * @brief 在背景中渲染可滚动纹理的组件 创建视察效果；
 */
class ParallaxComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    /**
     * @brief 构造函数
     * @param texture_id 图片id（路径）
     * @param scroll_factor 滚动因子
     * @param repeat 是否沿xy轴重复；
     */
    ParallaxComponent(const std::string& texture_id, const glm::vec2& scroll_factor, const glm::bvec2& repeat);
    ~ParallaxComponent() = default;

    [[nodiscard]] engine::render::Sprite getSprite() const { return sprite_; }
    [[nodiscard]] glm::vec2 getScrollFactor() const { return scroll_factor_; }
    [[nodiscard]] glm::bvec2 getRepeat() const { return repeat_; }
    [[nodiscard]] bool isHidden() const { return is_hidden_; }

    void setSprite(const engine::render::Sprite& sprite) { sprite_ = sprite; }
    void setScrollFactor(const glm::vec2& scroll_factor) { scroll_factor_ = scroll_factor; }
    void setRepeat(const glm::bvec2& repeat) { repeat_ = repeat; }
    void setIsHidden(bool is_hidden) { is_hidden_ = is_hidden; }

protected:
    void init() override;
    void render(engine::core::Context& context) override;
    void update(const std::chrono::milliseconds&, engine::core::Context&) override {};

private:
    engine::component::TransformComponent* transform_; // 位置变换组件；
    engine::render::Sprite sprite_;                    // 精灵对象

    glm::vec2 scroll_factor_; // 速度滚动因子;(0 = 禁止， 1 = 随相机移动 1< = 比相机慢)
    glm::bvec2 repeat_;       // 是否沿x，y轴重复
    bool is_hidden_{false};   // 是否隐藏;
};
} // component
} // engine

#endif //PARALLAX_COMPONENT_H
