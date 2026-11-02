//
// Created by wangm on 26-11-2.
//

#ifndef UI_PANEL_H
#define UI_PANEL_H
#include "ui_element.h"

namespace engine {
namespace ui {
/**
 * @brief 用于分组其他Ui元素的容器Ui元素；
 *
 * 通常用于布局和组织
 * 可以选择是否绘制背景色
 */
class UiPanel final : public engine::ui::UiElement {
public:
    UiPanel(const glm::vec2& position, const glm::vec2& size,
            const std::optional<engine::utils::FColor>& background_color = std::nullopt);

    ~UiPanel() override = default;

    // get/set
    [[nodiscard]] std::optional<engine::utils::FColor> getBackgroundColor() const { return background_color_; }

    void setBackgroundColor(const std::optional<engine::utils::FColor>& background_color) {
        this->background_color_ = background_color;
    }

    void render(engine::core::Context& context) override;

private:
    std::optional<engine::utils::FColor> background_color_; // 背景色
};
} // ui
} // engine

#endif //UI_PANEL_H
