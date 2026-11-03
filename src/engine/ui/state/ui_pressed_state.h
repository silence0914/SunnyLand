//
// Created by wangm on 26-11-3.
//

#ifndef UI_PRESSED_STATE_H
#define UI_PRESSED_STATE_H
#include "ui_state.h"

namespace engine {
namespace ui {
namespace state {
class UiPressedState final : public engine::ui::state::UiState {
    friend class engine::ui::UiInteractive;

public:
    explicit UiPressedState(engine::ui::UiInteractive* owner): UiState(owner) {}
    ~UiPressedState() override = default;

protected:
    void enter() override;
    std::unique_ptr<UiState> handleInput(engine::core::Context& context) override;
};
} // state
} // ui
} // engine

#endif //UI_PRESSED_STATE_H
