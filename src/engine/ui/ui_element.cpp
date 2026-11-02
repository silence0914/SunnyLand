//
// Created by wangm on 26-11-2.
//

#include "ui_element.h"
#include "../core/context.h"

namespace engine {
namespace ui {
UiElement::UiElement(const glm::vec2& position, const glm::vec2& size)
    : position_(position), size_(size) {}

bool UiElement::handleInput(engine::core::Context& context) {
    if (!visible_) { return false; }

    for (auto it = children_.begin(); it != children_.end();) {
        auto& item = *it;
        if (item->isNeedRemove()) {
            it = children_.erase(it);
        } else {
            if (item->handleInput(context)) { return true; }
            ++it;
        }
    }

    return false; // 事件未被消耗；
}

void UiElement::update(float dt, engine::core::Context& context) {
    if (!visible_) { return; }

    for (auto it = children_.begin(); it != children_.end();) {
        auto& item = *it;
        if (item->isNeedRemove()) {
            it = children_.erase(it);
        } else {
            item->update(dt, context);
            ++it;
        }
    }
}

void UiElement::render(engine::core::Context& context) {
    if (!visible_) { return; }
    for (auto& child: children_) {
        child->render(context);
    }
}

void UiElement::addChild(std::unique_ptr<UiElement> child) {
    if (child) {
        child->setParent(this);
        children_.emplace_back(std::move(child));
    }
}

std::unique_ptr<UiElement> UiElement::removeChild(UiElement* child_ptr) {
    auto it = std::find_if(children_.begin(), children_.end(),
                           [child_ptr](const std::unique_ptr<engine::ui::UiElement>& ptr) {
                               return ptr.get() == child_ptr;
                           });
    if (it == children_.end()) { return nullptr; }
    std::unique_ptr<UiElement> remove_child = std::move(*it);
    children_.erase(it);
    remove_child->setParent(nullptr);
    return remove_child;
}

void UiElement::removeAllChildren() {
    for (auto& child: children_) {
        child->removeAllChildren();
    }
    children_.clear();
}

engine::utils::Rect UiElement::getBounds() const {
    auto abs_pos = getScreenPosition();
    return engine::utils::Rect(abs_pos, size_);
}

glm::vec2 UiElement::getScreenPosition() const {
    if (!parent_) { return position_; }
    return parent_->getScreenPosition() + position_;
}

bool UiElement::isPointInside(const glm::vec2& point) const {
    auto bounds = getBounds();
    return point.x > bounds.position.x &&
            point.x < bounds.position.x + bounds.size.x &&
            point.y > bounds.position.y &&
            point.y < bounds.position.y + bounds.size.y;
}
} // ui
} // engine
