//
// Created by wangm on 26-10-14.
//

#include "game_object.h"

namespace engine {
namespace object {
GameObject::GameObject(std::string name, std::string tag)
    : name_(std::move(name)), tag_(std::move(tag)) {
    spdlog::trace("create GameObject '{}' '{}' completion", name_, tag_);
}

void GameObject::handleInput() {
    // 遍历处理所有组件输入
    for (const auto& component: components_) {
        component.second->handleInput();
    }
}

void GameObject::update(const std::chrono::milliseconds& dt) {
    // 遍历更新全部组件
    for (const auto& component: components_) {
        component.second->update(dt);
    }
}

void GameObject::render() {
    // 遍历渲染全部组件；
    for (const auto& component: components_) {
        component.second->render();
    }
}

void GameObject::clear() {
    // 遍历调用 组件 clear；
    for (const auto& component: components_) {
        component.second->clear();
    }
    // 清空容器， 自动释放内存；
    components_.clear();
}
} // object
} // engine
