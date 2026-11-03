//
// Created by wangm on 26-11-2.
//

#ifndef UI_MANAGER_H
#define UI_MANAGER_H
#include <memory>
#include <glm/vec2.hpp>

#include "ui_element.h"

namespace engine::core {
class Context;
}

namespace engine {
namespace ui {
class UiPanel;

class UiManager final {
public:
    UiManager();
    ~UiManager() = default;

    bool init(const glm::vec2& window_size);                         // 初始化Ui管理器  设置根元素大小；
    void addElement(std::unique_ptr<engine::ui::UiElement> element); // 添加一个元素到根节点的child_容器中
    UiPanel* getRootElement() const { return root_element_.get(); }  // 获取根UiPanel元素的指针；
    void clearElements();                                            // 清除所有ui元素；

    // 核心循环
    bool handleInput(engine::core::Context& context);
    void update(float dt, engine::core::Context& context);
    void render(engine::core::Context& context);

    UiManager(const UiManager& other) = delete;
    UiManager(UiManager&& other) noexcept = delete;
    UiManager& operator=(const UiManager& other) = delete;
    UiManager& operator=(UiManager&& other) noexcept = delete;

private:
    std::unique_ptr<UiPanel> root_element_; // 一个UiPanel 作为根节点；
};
} // ui
} // engine

#endif //UI_MANAGER_H
