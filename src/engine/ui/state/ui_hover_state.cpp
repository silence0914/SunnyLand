//
// Created by wangm on 26-11-3.
//

#include "ui_hover_state.h"

#include <spdlog/spdlog.h>

#include "ui_normal_state.h"
#include "ui_pressed_state.h"
#include "../ui_interactive.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"

namespace engine {
namespace ui {
namespace state {
void UiHoverState::enter() {
    owner_->setSprite("hover");
}

std::unique_ptr<UiState> UiHoverState::handleInput(engine::core::Context& context) {
    auto& input_manager = context.getInputManager();
    auto mouse_position = input_manager.getMousePosition();
    if (!owner_->isPointInside(mouse_position)) {
        return std::make_unique<engine::ui::state::UiNormalState>(owner_);
    }
    if (input_manager.isActionPressed("MouseLeftClick")) {
        return std::make_unique<UiPressedState>(owner_);
    }
    return nullptr;
}
} // state
} // ui
} // engine
