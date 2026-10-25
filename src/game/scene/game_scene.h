//
// Created by wangm on 26-10-18.
//

#ifndef GAME_SCENE_H
#define GAME_SCENE_H
#include "../../engine/scene/scene.h"

namespace engine::object {
class GameObject;
}

namespace game::scene {
class GameScene final : public engine::scene::Scene {
public:
    GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& scene_manager);
    ~GameScene() = default;

    GameScene(const GameScene& other) = delete;
    GameScene(GameScene&& other) noexcept = delete;
    GameScene& operator=(const GameScene& other) = delete;
    GameScene& operator=(GameScene&& other) noexcept = delete;

    // 核心循环；
    void init() override;
    void handleInput() override;
    void update(const std::chrono::milliseconds& dt) override;
    void render() override;
    void clean() override;

private:
    [[nodiscard]] bool initLevel();
    [[nodiscard]] bool initPlayer();

private:
    engine::object::GameObject* player_{nullptr};
};
} // scene

#endif //GAME_SCENE_H
