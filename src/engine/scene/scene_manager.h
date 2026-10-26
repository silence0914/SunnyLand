//
// Created by wangm on 26-10-14.
//

#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <memory>
#include <vector>
#include <chrono>


namespace engine::core {
class Context;
}

namespace engine::scene {
class Scene;

/**
 * @brief 管理游戏中的场景栈，处理场景切换和生命周期
 */
class SceneManager final {
    enum class PendingAction { None, Push, Pop, Replace };

public:
    explicit SceneManager(engine::core::Context& context);
    ~SceneManager();

    SceneManager(const SceneManager& other) = delete;
    SceneManager(SceneManager&& other) noexcept = delete;
    SceneManager& operator=(const SceneManager& other) = delete;
    SceneManager& operator=(SceneManager&& other) noexcept = delete;

    // 循环
    void handelInput();
    void update(float dt);
    void render();

    void close();

    // 延时切换场景
    void requestPushScene(std::unique_ptr<engine::scene::Scene>&& scene);    // 请求压入新场景
    void requestPopScene();                                                  // 请求弹出当前场景
    void requestReplaceScene(std::unique_ptr<engine::scene::Scene>&& scene); // 请求替换当前场景

    // get
    [[nodiscard]] engine::core::Context& getContext() const { return context_; } // 获取上下文
    [[nodiscard]] engine::scene::Scene* getCurrentScene() const;                 // 获取当前场景

private:
    void processPendingActions();
    void pushScene();
    void popScene();
    void replaceScene();

private:
    engine::core::Context& context_;                                   // 上下文
    std::vector<std::unique_ptr<engine::scene::Scene> > scenes_stack_; // 场景栈

    PendingAction pending_action_{PendingAction::None};   // 待处理的状态
    std::unique_ptr<engine::scene::Scene> pending_scene_; // 待处理的场景
};
} // scene
// engine

#endif //SCENE_MANAGER_H
