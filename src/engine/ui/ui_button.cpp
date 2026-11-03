//
// Created by wangm on 26-11-3.
//

#include "ui_button.h"

#include "state/ui_normal_state.h"

namespace engine {
namespace ui {
UiButton::UiButton(engine::core::Context& context,
                   const std::string& normal_sprite_id,
                   const std::string& hover_sprite_id,
                   const std::string& pressed_sprite_id,
                   const glm::vec2& position,
                   const glm::vec2& size,
                   const std::function<void()>& callback)
    : UiInteractive(context, position, size), callback_(std::move(callback)) {
    addSprite("normal", std::make_unique<engine::render::Sprite>(normal_sprite_id));
    addSprite("hover", std::make_unique<engine::render::Sprite>(hover_sprite_id));
    addSprite("pressed", std::make_unique<engine::render::Sprite>(pressed_sprite_id));

    // 初始状态设置为 normal;
    setState(std::make_unique<engine::ui::state::UiNormalState>(this));
    // 添加需要的音效；
    addSound("hover", "assets/audio/button_hover.wav");
    addSound("pressed", "assets/audio/button_click.wav");
}

void UiButton::clicked() {
    if (callback_) { callback_(); }
}
} // UI
} // engine
