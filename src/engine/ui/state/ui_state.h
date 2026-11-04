//
// Created by wangm on 26-11-3.
//

#ifndef UI_STATE_H
#define UI_STATE_H
#include <memory>
#include <string_view>

namespace engine::core {
class Context;
}

namespace engine::ui {
class UiInteractive;
}

namespace engine {
namespace ui {
namespace state {
constexpr std::string_view HOVER{"hover"};
constexpr std::string_view NORMAL{"normal"};
constexpr std::string_view PRESSED{"pressed"};
/**
 * @brief 可交互元素再特定状态下的行为接口
 */
class UiState {
    friend class engine::ui::UiInteractive;

public:
    explicit UiState(engine::ui::UiInteractive* owner): owner_(owner) {}
    virtual ~UiState() = default;

    UiState(const UiState& other) = delete;
    UiState(UiState&& other) noexcept = delete;
    UiState& operator=(const UiState& other) = delete;
    UiState& operator=(UiState&& other) noexcept = delete;

protected:
    virtual void enter() {}
    virtual std::unique_ptr<UiState> handleInput(engine::core::Context& context) = 0;

protected:
    engine::ui::UiInteractive* owner_ = nullptr;
};
} // state
} // ui
} // engine

#endif //UI_STATE_H
