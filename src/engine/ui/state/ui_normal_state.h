//
// Created by wangm on 26-11-3.
//

#ifndef UI_NORMAL_STATE_H
#define UI_NORMAL_STATE_H
#include "ui_state.h"

namespace engine {
namespace ui {
namespace state {
/**
 * @brief 正常状态  ui元素的默认状态
 */
class UiNormalState final : public engine::ui::state::UiState {
    friend class engine::ui::UiInteractive;

public:
    explicit UiNormalState(engine::ui::UiInteractive* owner): UiState(owner) {}
    ~UiNormalState() override = default;

protected:
    void enter() override;
    std::unique_ptr<UiState> handleInput(engine::core::Context& context) override;
};
} // state
} // ui
} // engine

#endif //UI_NORMAL_STATE_H
