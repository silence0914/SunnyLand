//
// Created by wangm on 26-10-14.
//

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "../component/component.h"
#include <string>
#include <unordered_map>
#include <typeindex>
#include <spdlog/spdlog.h>

namespace engine::core {
class Context;
}

namespace engine::object {
template<typename T>
concept derived_component = std::derived_from<T, engine::component::Component>; // 约束模板参数T 必须继承自Component;
/**
 * @brief 游戏对象类
 * 管理对象组件
 */
class GameObject final {
public:
    GameObject(std::string name = "", std::string tag = "");

    GameObject(const GameObject& other) = delete;
    GameObject(GameObject&& other) noexcept = delete;
    GameObject& operator=(const GameObject& other) = delete;
    GameObject& operator=(GameObject&& other) noexcept = delete;

    // get/set
    [[nodiscard]] std::string getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    [[nodiscard]] std::string getTag() const { return tag_; }
    void setTag(const std::string& tag) { tag_ = tag; }
    [[nodiscard]] bool isNeedRemove() const { return need_remove_; }
    void setNeedRemove(bool need_remove) { need_remove_ = need_remove; }

    // 循环组件；
    void handleInput(engine::core::Context& context);                                 // 处理输入
    void update(float dt, engine::core::Context& context); // 更新所有组件
    void render(engine::core::Context& context);                                      // 渲染所有组件

    void clear(); // 清理组件

    /**
     * @brief 添加组件
     * @tparam T 组件类型  -  必须继承自component；
     * @tparam Args 组件构造函数参数类型
     * @param args 组件构造函数参数实参
     * @return
     */
    template<typename T, typename... Args>
        requires derived_component<T>
    T* addComponent(Args&&... args) {
        // 如果组件已经存在  直接返回
        if (hasComponent<T>()) {
            return getComponent<T>();
        }

        auto type_index = std::type_index(typeid(T));
        auto new_component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = new_component.get();
        new_component->setOwner(this);
        components_[type_index] = std::move(new_component);
        ptr->init();
        spdlog::debug("GameObject '{}' addComponent '{}' component ", name_, typeid(T).name());
        return ptr;
    }

    /**
     * @brief 检查组件是否已经存在
     * @tparam T 组件类型
     * @return
     */
    template<typename T>
        requires derived_component<T>
    [[nodiscard]] bool hasComponent() const {
        return components_.contains(std::type_index(typeid(T)));
    }

    /**
     * @brief 获取组件
     * @tparam T 组件类型
     * @return 组件指针
     */
    template<typename T>
        requires derived_component<T>
    T* getComponent() const {
        auto it = components_.find(std::type_index(typeid(T)));
        if (it == components_.end()) {
            return nullptr;
        }
        return static_cast<T*>(it->second.get());
    }

    /**
     * @brief 删除组件
     * @tparam T 组件类型
     * @return 被删除的组件指针
     */
    template<typename T>
        requires derived_component<T>
    void removeComponent() {
        auto type_index = std::type_index(typeid(T));
        auto it = components_.find(type_index);
        if (it == components_.end()) {
            return;
        }
        it->second->clear();
        components_.erase(it);
    }

private:
    std::string name_;                                                                               // 名称
    std::string tag_;                                                                                // 标签
    std::unordered_map<std::type_index, std::unique_ptr<engine::component::Component> > components_; // 组件表
    bool need_remove_{false};                                                                        // 延迟删除标识；
};
} // object
// engine

#endif //GAME_OBJECT_H
