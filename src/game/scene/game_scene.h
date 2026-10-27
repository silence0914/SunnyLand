//
// Created by wangm on 26-10-18.
//

#ifndef GAME_SCENE_H
#define GAME_SCENE_H
#include <glm/vec2.hpp>

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
    void update(float dt) override;
    void render() override;
    void clean() override;

private:
    [[nodiscard]] bool initLevel();
    [[nodiscard]] bool initPlayer();
    [[nodiscard]] bool initEnemyAndItem();

    void handleObjectCollisions();                                                                      // 处理游戏对象间的碰撞
    void handleTileTriggers();                                                                          // 处理瓦片触发事件；
    void playerVsEnemyCollision(engine::object::GameObject& player, engine::object::GameObject& enemy); // 玩家与敌人的碰撞
    void playerVsItemCollision(engine::object::GameObject& player, engine::object::GameObject& item);   // 玩家与道具间碰撞；

    /**
     * @brief 创建一个特效对象；
     * @param center_pos 特效中心位置；
     * @param tag 特效标签
     */
    void createEffect(const glm::vec2& center_pos, const std::string& tag);

private:
    engine::object::GameObject* player_{nullptr};
};
} // scene

#endif //GAME_SCENE_H
