//
// Created by wangm on 26-11-3.
//

#ifndef UI_INTERACTIVE_H
#define UI_INTERACTIVE_H
#include "ui_element.h"
#include "../render/renderer.h"
#include "state/ui_state.h"

namespace engine {
namespace ui {
/**
 * @brief 可交互ui 的基类
 */
class UiInteractive : public engine::ui::UiElement {
public:
    explicit UiInteractive(engine::core::Context& context, const glm::vec2& position = {0.0f, 0.0f},
                           const glm::vec2& size = {0.0f, 0.0f});
    ~UiInteractive() override = default;

    virtual void clicked() {}                                                                // 点击事件；
    void addSprite(const std::string& name, std::unique_ptr<engine::render::Sprite> sprite); // 添加精灵
    void setSprite(const std::string& name);                                                 // 设置当前显示的精灵
    void addSound(const std::string& name, const std::string& path);                         // 添加音效；
    void playSound(const std::string& name);                                                 // 播放音效

    bool handleInput(engine::core::Context& context) override;
    void render(engine::core::Context& context) override;

    // get/set
    [[nodiscard]] bool isInteractive() const { return interactive_; }
    void setInteractive(bool interactive) { interactive_ = interactive; }
    [[nodiscard]] engine::ui::state::UiState* getState() const { return state_.get(); }
    void setState(std::unique_ptr<engine::ui::state::UiState> state);

private:
    engine::core::Context& context_;
    std::unique_ptr<engine::ui::state::UiState> state_;                                 // 当前状态；
    std::unordered_map<std::string, std::unique_ptr<engine::render::Sprite> > sprites_; // 精灵集合
    std::unordered_map<std::string, std::string> sounds_;                               // 音效集合 key为音效名  value为音效文件路径
    engine::render::Sprite* current_sprite_{nullptr};                                   // 当前显示的精灵
    bool interactive_ = true;                                                           //是否可交互
};
} // ui
} // engine

#endif //UI_INTERACTIVE_H
