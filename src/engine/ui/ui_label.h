//
// Created by wangm on 26-11-2.
//

#ifndef UI_LABEL_H
#define UI_LABEL_H
#include "ui_element.h"

namespace engine::render {
class TextRenderer;
}

namespace engine {
namespace ui {
class UiLabel final : public engine::ui::UiElement {
public:
    UiLabel(engine::render::TextRenderer& text_renderer, const std::string& text, const std::string& font_id,
            int font_size, const engine::utils::FColor& text_color = {1.0f, 1.0f, 1.0f, 1.0f},
            const glm::vec2& position = {0.0f, 0.0f});

    ~UiLabel() = default;

    void render(engine::core::Context& context) override;

    [[nodiscard]] const std::string& getText() const { return text_; }
    [[nodiscard]] const std::string& getFontId() const { return font_id_; }
    [[nodiscard]] int getFontSize() const { return font_size_; }
    [[nodiscard]] const engine::utils::FColor& getTextColor() const { return text_color; }

    void setText(const std::string& text);
    void setFontId(const std::string& font_id);
    void setFontSize(int font_size);
    void setTextColor(const engine::utils::FColor& text_color) { this->text_color = text_color; }

private:
    void setSize();

private:
    engine::render::TextRenderer& text_renderer_;

    std::string text_;                // 文本内容；
    std::string font_id_;             // 字体id;
    int font_size_;                   // 字体大小;
    engine::utils::FColor text_color; // 字体颜色;
};
} // ui
} // engine

#endif //UI_LABEL_H
