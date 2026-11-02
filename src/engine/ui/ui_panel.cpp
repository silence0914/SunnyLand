//
// Created by wangm on 26-11-2.
//

#include "ui_panel.h"

#include "../core/context.h"
#include "../render/renderer.h"

namespace engine {
namespace ui {
UiPanel::UiPanel(const glm::vec2& position, const glm::vec2& size,
                 const std::optional<engine::utils::FColor>& background_color)
    : UiElement(position, size), background_color_(background_color) {}

void UiPanel::render(engine::core::Context& context) {
    if (!visible_) { return; }
    if (background_color_.has_value()) {
        context.getRenderer().drawUiFilledRect(getBounds(), background_color_.value());
    }

    UiElement::render(context);
}
} // ui
} // engine
