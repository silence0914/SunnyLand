//
// Created by wangm on 26-11-2.
//

#include "ui_image.h"

#include "../core/context.h"
#include "../render/renderer.h"

namespace engine {
namespace ui {
UiImage::UiImage(const std::string& texture_id, const glm::vec2& position, const glm::vec2& size,
                 const std::optional<SDL_FRect>& source_rect, bool is_flipped)
    : sprite_(texture_id, source_rect, is_flipped), UiElement(position, size) {
    if (sprite_.getTextureId().empty()) {
        visible_ = false;
    }
}

void UiImage::render(engine::core::Context& context) {
    if (!visible_) { return; }
    auto position = getScreenPosition();
    // 如果尺寸为0  使用纹理的原始尺寸；
    if (size_.x == 0.0f && size_.y == 0.0f) {
        context.getRenderer().drawUISprite(sprite_, position);
    } else {
        context.getRenderer().drawUISprite(sprite_, position, size_);
    }

    // 渲染子元素
    UiElement::render(context);
}

void UiImage::setVisible(bool visible) {
    if (sprite_.getTextureId().empty()) {
        visible_ = false;
    }
    visible_ = visible;
}
} // ui
} // engine
