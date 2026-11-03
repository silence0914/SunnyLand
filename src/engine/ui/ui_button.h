//
// Created by wangm on 26-11-3.
//

#ifndef UI_BUTTON_H
#define UI_BUTTON_H
#include "ui_interactive.h"

namespace engine {
namespace ui {
/**
 * @brief ui 按钮元素
 *
 * 支持正常状态   悬停   按下
 */
class UiButton final : public engine::ui::UiInteractive {
public:
    /**
     * @brief 构造函数
     * @param context 上下文
     * @param normal_sprite_id 正常状态精灵id
     * @param hover_sprite_id 悬停状态精灵id
     * @param pressed_sprite_id 按下状态精灵id
     * @param position 位置
     * @param size 大小
     * @param callback 回调函数
     */
    UiButton(engine::core::Context& context,
             const std::string& normal_sprite_id,
             const std::string& hover_sprite_id,
             const std::string& pressed_sprite_id,
             const glm::vec2& position = {0.0f, 0.0f},
             const glm::vec2& size = {0.0f, 0.0f},
             const std::function<void()>& callback = nullptr);

    ~UiButton() override = default;
    void clicked() override;
    void setCallback(std::function<void()> callback) { callback_ = std::move(callback); }
    [[nodiscard]] std::function<void()> getCallback() const { return callback_; }

private:
    std::function<void()> callback_;
};
} // UI
} // engine

#endif //UI_BUTTON_H
