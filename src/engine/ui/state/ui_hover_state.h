//
// Created by wangm on 26-11-3.
//

#ifndef UI_HOVER_STATE_H
#define UI_HOVER_STATE_H
#include "ui_state.h"

namespace engine {
namespace ui {
namespace state {

class UiHoverState final : public engine::ui::state::UiState{
public:
    explicit UiHoverState(engine::ui::UiInteractive* owner): UiState(owner) {}
    ~UiHoverState() override = default;

protected:
    void enter() override;
    std::unique_ptr<UiState> handleInput(engine::core::Context& context) override;
};

} // state
} // ui
} // engine

#endif //UI_HOVER_STATE_H