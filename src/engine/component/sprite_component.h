//
// Created by wangm on 26-10-14.
//

#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <glm/vec2.hpp>

#include "component.h"
#include "../render/sprite.h"
#include "../utils/alignment.h"

namespace engine::resource {
class ResourceManager;
}

namespace engine {
namespace component {
class TransformComponent;

/**
 * @brief 持有sprite对象 管理GameObject 的视觉表示
 *
 * 协调sprite 数据和渲染逻辑  与TransformComponent交互；
 */
class SpriteComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    /**
     * @brief 构造函数
     * @param texture_id  纹理id
     * @param resource_manager 资源管理器指针
     * @param alignment 初始对齐方式 可选
     * @param source_rect 原矩形 可选
     * @param is_flipped 初始翻转状态；
     */
    SpriteComponent(const std::string& texture_id,
                    engine::resource::ResourceManager* resource_manager,
                    engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
                    std::optional<SDL_FRect> source_rect = std::nullopt,
                    bool is_flipped = false
    );

    /**
     * @brief 构造函数
     * @param sprite 精灵对象
     * @param resource_manager 资源管理器
     * @param alignment 初始对齐方式
     */
    SpriteComponent(engine::render::Sprite&& sprite,
                    engine::resource::ResourceManager& resource_manager,
                    engine::utils::Alignment alignment = engine::utils::Alignment::NONE
    );

    ~SpriteComponent() override = default;

    SpriteComponent(const SpriteComponent& other) = delete;
    SpriteComponent(SpriteComponent&& other) noexcept = delete;
    SpriteComponent& operator=(const SpriteComponent& other) = delete;
    SpriteComponent& operator=(SpriteComponent&& other) noexcept = delete;

    void updateOffset(); // 更新偏移量，  根据当前对齐防守 alignment, 尺寸大小 sprite_size 计算；

    // get/set
    [[nodiscard]] const engine::render::Sprite& getSprite() const { return sprite_; }
    [[nodiscard]] const std::string& getTexture() const { return sprite_.getTextureId(); }
    [[nodiscard]] engine::utils::Alignment getAlignment() const { return alignment_; }
    [[nodiscard]] glm::vec2 getSpriteSize() const { return sprite_size_; }
    [[nodiscard]] glm::vec2 getOffset() const { return offset_; }
    [[nodiscard]] bool isIsFlipped() { return sprite_.isIsFlipped(); }
    [[nodiscard]] bool isIsHidden() const { return is_hidden_; }

    void setSpriteById(const std::string& texture_id, const std::optional<SDL_FRect>& source_rect = std::nullopt);
    void setIsFlipped(bool is_flipped) { sprite_.setIsFlipped(is_flipped); } // 设置翻转
    void setIsHidden(bool is_hidden) { is_hidden_ = is_hidden; }             // 设置隐藏
    void setSourceRect(const std::optional<SDL_FRect>& source_rect);         // 设置精灵图原矩形；
    void setAlignment(engine::utils::Alignment alignment);

private:
    void updateSpriteSize(); // 根据sprite_ 的 source_size 更新 sprite_size;

    void init() override;
    void update(const float, engine::core::Context&) {};
    void render(engine::core::Context& context) override;

private:
    engine::resource::ResourceManager* resource_manager_{nullptr};
    engine::component::TransformComponent* transform_component_{nullptr};

    engine::render::Sprite sprite_;                                      // 精灵对象
    engine::utils::Alignment alignment_{engine::utils::Alignment::NONE}; // 对齐方式
    glm::vec2 sprite_size_{};                                            // 精灵尺寸
    glm::vec2 offset_{};                                                 // 偏移量
    bool is_hidden_{false};                                              // 是否隐藏
};
} // component
} // engine

#endif //SPRITE_COMPONENT_H
