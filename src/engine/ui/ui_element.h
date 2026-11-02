//
// Created by wangm on 26-11-2.
//

#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H
#include <memory>
#include <vector>
#include <glm/vec2.hpp>

#include "../utils/action.h"
#include "../utils/math.h"


namespace engine::core {
class Context;
}

namespace engine {
namespace ui {
/**
 * @brief 所有ui的基类
 *
 * 管理ui元素的 位置 大小 可见性  状态；
 *  提供事件处理，更新，渲染的虚方法
 */
class UiElement {
public:
    explicit UiElement(const glm::vec2& position = {0.0f, 0.0f}, const glm::vec2& size = {0.0f, 0.0f});

    virtual ~UiElement() = default;

    // 核心循环
    virtual bool handleInput(engine::core::Context& context);
    virtual void update(float dt, engine::core::Context& context);
    virtual void render(engine::core::Context& context);

    // 层级结构管理
    void addChild(std::unique_ptr<UiElement> child);              // 添加子元素
    std::unique_ptr<UiElement> removeChild(UiElement* child_ptr); // 将子元素移出列表并返回其智能指针
    void removeAllChildren();                                     // 清空所有子元素；

    // get/set
    [[nodiscard]] const glm::vec2& getPosition() const { return position_; }
    [[nodiscard]] const glm::vec2& getSize() const { return size_; }
    [[nodiscard]] bool isVisible() const { return visible_; }
    [[nodiscard]] bool isNeedRemove() const { return need_remove_; }
    [[nodiscard]] UiElement* getParent() const { return parent_; }
    [[nodiscard]] const std::vector<std::unique_ptr<UiElement> >& getChildren() const { return children_; }

    void setPosition(const glm::vec2& position) { position_ = position; }
    void setSize(const glm::vec2& size) { this->size_ = size; }
    virtual void setVisible(bool visible) { visible_ = visible; }
    void setNeedRemove(bool need_remove) { need_remove_ = need_remove; }
    void setParent(UiElement* parent) { parent_ = parent; }

    // 辅助方法
    engine::utils::Rect getBounds() const;            // 获取元素边界 （屏幕坐标）
    glm::vec2 getScreenPosition() const;              // 获取元素在屏幕上位置；
    bool isPointInside(const glm::vec2& point) const; // 检查给定点是否在元素的边界内；

    UiElement(const UiElement& other) = delete;
    UiElement(UiElement&& other) noexcept = delete;
    UiElement& operator=(const UiElement& other) = delete;
    UiElement& operator=(UiElement&& other) noexcept = delete;

protected:
    glm::vec2 position_{0.0f}; // 相对于父元素的局部位置
    glm::vec2 size_{0.0f};     // 元素大小
    bool visible_{true};       // 是否可见
    bool need_remove_{false};  // 是否需要延迟删除

    UiElement* parent_{nullptr};                        // 父元素指针
    std::vector<std::unique_ptr<UiElement> > children_; // 子元素列表；
};
} // ui
} // engine

#endif //UI_ELEMENT_H
