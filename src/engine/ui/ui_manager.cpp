//
// Created by wangm on 26-11-2.
//

#include "ui_manager.h"

#include "ui_panel.h"

namespace engine {
namespace ui {
UiManager::UiManager() {
    root_element_ = std::make_unique<engine::ui::UiPanel>(glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
}

bool UiManager::init(const glm::vec2& window_size) {
    root_element_->setSize(window_size);
    return true;
}

void UiManager::addElement(std::unique_ptr<engine::ui::UiElement> element) {
    if (!element) { return; }
    root_element_->addChild(std::move(element));
}

void UiManager::clearElements() {
    root_element_->removeAllChildren();
}

bool UiManager::handleInput(engine::core::Context& context) {
    if (!root_element_->isVisible()) { return false; }
    return root_element_->handleInput(context);
}

void UiManager::update(float dt, engine::core::Context& context) {
    if (!root_element_->isVisible()) { return; }
    root_element_->update(dt, context);
}

void UiManager::render(engine::core::Context& context) {
    if (!root_element_->isVisible()) { return; }
    root_element_->render(context);
}
} // ui
} // engine
