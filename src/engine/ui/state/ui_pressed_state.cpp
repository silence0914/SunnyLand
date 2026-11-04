//
// Created by wangm on 26-11-3.
//

#include "ui_pressed_state.h"

#include "ui_hover_state.h"
#include "ui_normal_state.h"
#include "../ui_interactive.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"

namespace engine {
namespace ui {
namespace state {
void UiPressedState::enter() {
    owner_->setSprite(PRESSED);
    owner_->playSound(std::string{PRESSED});
}

std::unique_ptr<UiState> UiPressedState::handleInput(engine::core::Context& context) {
    auto& input_manager = context.getInputManager();
    auto mouse_position = input_manager.getMousePosition();
    // 松开左键
    if (input_manager.isActionReleased("MouseLeftClick")) {
        // 如果松开时，光标在按键范围内
        if (owner_->isPointInside(mouse_position)) {
            // 触发点击事件；
            owner_->clicked();
            return std::make_unique<engine::ui::state::UiHoverState>(owner_);
        } else {
            // 松开时不在按键范围内   回到normale state;
            return std::make_unique<engine::ui::state::UiNormalState>(owner_);
        }
    }
    return nullptr;
}
} // state
} // ui
} // engine
