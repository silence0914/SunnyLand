//
// Created by wangm on 26-11-2.
//

#include "ui_label.h"
#include "../render/text_renderer.h"

namespace engine {
namespace ui {
UiLabel::UiLabel(engine::render::TextRenderer& text_renderer, const std::string& text, const std::string& font_id,
                 int font_size, const engine::utils::FColor& text_color, const glm::vec2& position)
    : UiElement(position),
      text_renderer_(text_renderer), text_(text), font_id_(font_id), font_size_(font_size), text_color(text_color) {
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_);
}

void UiLabel::render(engine::core::Context& context) {
    if (!visible_ || text_.empty()) { return; }
    text_renderer_.drawUiText(text_, font_id_, font_size_, getScreenPosition(), text_color);
    UiElement::render(context);
}

void UiLabel::setText(const std::string& text) {
    text_ = text;
    setSize();
}

void UiLabel::setFontId(const std::string& font_id) {
    font_id_ = font_id;
    setSize();
}

void UiLabel::setFontSize(int font_size) {
    font_size_ = font_size;
    setSize();
}

void UiLabel::setSize() {
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_);
}
} // ui
} // engine
