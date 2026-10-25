//
// Created by wangm on 26-10-14.
//

#ifndef COMPONENT_H
#define COMPONENT_H

#include <chrono>

namespace engine::object {
class GameObject;
}

namespace engine::core {
class Context;
}

namespace engine::component {
/**
 * @brief 组件抽象基类
 *
 * 定义了组件通用方法， 基础循环
 */
class Component {
    friend class engine::object::GameObject;

public:
    Component() = default;
    virtual ~Component() = default;

    Component(const Component& other) = delete;
    Component(Component&& other) noexcept = delete;
    Component& operator=(const Component& other) = delete;
    Component& operator=(Component&& other) noexcept = delete;

    // get/set
    [[nodiscard]] engine::object::GameObject* getOwner() const { return owner_; }
    void setOwner(engine::object::GameObject* owner) { owner_ = owner; }

protected:
    engine::object::GameObject* owner_{}; // 指向拥有此组件的 GameObject 对象；

protected:
    virtual void init() {} //初始化
    virtual void handleInput(engine::core::Context&) {} // 输入处理
    virtual void update(const std::chrono::milliseconds& ,engine::core::Context&) {} // 更新
    virtual void render(engine::core::Context&) {} // 渲染
    virtual void clear() {} // 清理
};
} // component
// engine

#endif //COMPONENT_H
