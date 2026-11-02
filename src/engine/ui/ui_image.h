//
// Created by wangm on 26-11-2.
//

#ifndef UI_IMAGE_H
#define UI_IMAGE_H
#include "ui_element.h"
#include "../render/sprite.h"

namespace engine {
namespace ui {
class UiImage final : public engine::ui::UiElement {
public:
    UiImage(const std::string& texture_id,
            const glm::vec2& position = {0.0f, 0.0f},
            const glm::vec2& size = {0.0f, 0.0f},
            const std::optional<SDL_FRect>& source_rect = std::nullopt,
            bool is_flipped = false);

    ~UiImage() = default;

    void render(engine::core::Context& context) override;

    // get/set
    [[nodiscard]] engine::render::Sprite getSprite() const { return sprite_; }
    [[nodiscard]] const std::string& getTextureId() const { return sprite_.getTextureId(); }
    [[nodiscard]] const std::optional<SDL_FRect>& getSourceRect() const { return sprite_.getSourceRect(); }
    [[nodiscard]] bool isFlipped() { return sprite_.isFlipped(); }

    void setSprite(const engine::render::Sprite& sprite) { sprite_ = sprite; }
    void setTextureId(const std::string& texture_id) { sprite_.setTextureId(texture_id); }
    void setSourceRect(const std::optional<SDL_FRect> source_rect) { sprite_.setSourceRect(source_rect); }
    void setIsFlipped(bool is_flipped) { sprite_.setIsFlipped(is_flipped); }

    void setVisible(bool visible) override;

protected:
    engine::render::Sprite sprite_;
};
} // ui
} // engine

#endif //UI_IMAGE_H
