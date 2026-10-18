//
// Created by wangm on 26-10-14.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>
#include <chrono>


namespace engine::object {
class GameObject;
}

namespace engine::scene {
class SceneManager;
}

namespace engine::render {
class Camera;
}

namespace engine::core {
class Context; // 上下文；
}

namespace engine::scene {
/**
 * @brief 场景基类
 *
 * 负责管理场景中的游戏对象和场景的生命周期
 */
class Scene {
public:
    Scene(std::string scene_name, engine::core::Context& context, engine::scene::SceneManager& scene_manager);
    virtual ~Scene() = default;

    Scene(const Scene& other) = delete;
    Scene(Scene&& other) noexcept = delete;
    Scene& operator=(const Scene& other) = delete;
    Scene& operator=(Scene&& other) noexcept = delete;

    virtual void init();
    virtual void handleInput();
    virtual void update(const std::chrono::milliseconds& dt);
    virtual void render();
    virtual void clean();

    virtual void addGameObject(std::unique_ptr<engine::object::GameObject>&& game_object);     // 向场景中添加游戏对象
    virtual void safeAddGameObject(std::unique_ptr<engine::object::GameObject>&& game_object); // 向场景中安全添加游戏对象

    virtual void removeGameObject(engine::object::GameObject* game_object);     // 删除对象
    virtual void safeRemoveGameObject(engine::object::GameObject* game_object); // 安全删除对象


    engine::object::GameObject* findGameObjectByName(const std::string& game_object_name);

    // get/set
    [[nodiscard]] std::string getSceneName() const { return scene_name_; }
    [[nodiscard]] bool isInitialized() const { return is_initialized_; }
    [[nodiscard]] engine::core::Context& getContext() const { return context_; }
    [[nodiscard]] engine::scene::SceneManager& getSceneManager() const { return scene_manager_; }

    [[nodiscard]] const std::vector<std::unique_ptr<engine::object::GameObject> >& getGameObjects() const {
        return game_objects_;
    }

    void setSceneName(const std::string& scene_name) { scene_name_ = scene_name; }
    void setIsInitialized(bool is_initialized) { is_initialized_ = is_initialized; }

protected:
    void processPendingAdditions(); // pending_additions 中的对象加入到 game_objects 中
protected:
    std::string scene_name_{};                   // 场景名；
    engine::core::Context& context_;             // 上下文
    bool is_initialized_{false};                  // 是否初始化
    engine::scene::SceneManager& scene_manager_; // 场景管理器

    std::vector<std::unique_ptr<engine::object::GameObject> > game_objects_;     // 场景中的对象；
    std::vector<std::unique_ptr<engine::object::GameObject> > pending_additions; // 待添加场景
};
} // scene
// engine

#endif //SCENE_H
