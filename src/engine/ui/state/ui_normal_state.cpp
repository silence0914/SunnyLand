//
// Created by wangm on 26-11-3.
//

#include "ui_normal_state.h"
#include "ui_hover_state.h"
#include "../ui_interactive.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"

namespace engine {
namespace ui {
namespace state {
void UiNormalState::enter() {
    owner_->setSprite("normal");
}

std::unique_ptr<UiState> UiNormalState::handleInput(engine::core::Context& context) {
    auto& input_manager = context.getInputManager();
    auto mouse_position = input_manager.getMousePosition();
    if (owner_->isPointInside(mouse_position)) {
        owner_->playSound("hover");
        return std::make_unique<engine::ui::state::UiHoverState>(owner_);
    }
    return nullptr;
}
} // state
} // ui
} // engine
