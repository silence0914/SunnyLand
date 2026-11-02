//
// Created by wangm on 26-10-18.
//

#ifndef GAME_SCENE_H
#define GAME_SCENE_H
#include <glm/vec2.hpp>

#include "../../engine/scene/scene.h"

namespace engine::ui {
class UiPanel;
class UiLabel;
}

namespace engine::object {
class GameObject;
}

namespace game::data {
class SessionData;
}

namespace game::scene {
class GameScene final : public engine::scene::Scene {
public:
    GameScene(engine::core::Context& context,
              engine::scene::SceneManager& scene_manager,
              std::shared_ptr<game::data::SessionData> data = nullptr);
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
    [[nodiscard]] bool initUi();

    void handleObjectCollisions();                                                                      // 处理游戏对象间的碰撞
    void handleTileTriggers();                                                                          // 处理瓦片触发事件；
    void handlePlayerDamage(int damage);                                                                // 处理玩家受伤
    void playerVsEnemyCollision(engine::object::GameObject& player, engine::object::GameObject& enemy); // 玩家与敌人的碰撞
    void playerVsItemCollision(engine::object::GameObject& player, engine::object::GameObject& item);   // 玩家与道具间碰撞；
    void toNextLevel(engine::object::GameObject* trigger);                                              // 进入下一关
    std::string levelNameToPath(const std::string& level_name) const { return "assets/maps/" + level_name + ".tmj"; }

    /**
     * @brief 创建一个特效对象；
     * @param center_pos 特效中心位置；
     * @param tag 特效标签
     */
    void createEffect(const glm::vec2& center_pos, const std::string& tag);

    // ui
    void createScoreUi();           // 创建积分ui
    void createHealthUi();          // 创建生命值ui
    void addScoreWithUi(int score); // 增加得分
    void healWithUi(int amount);    // 变更生命
    void updateHealthWithUi();      // 更新生命值ui；

private:
    engine::object::GameObject* player_{nullptr};
    std::shared_ptr<game::data::SessionData> game_session_data_; // 场景间数据共享；
    engine::ui::UiLabel* score_label_{nullptr};                   // 得分标签, 生命周期由UiManager管理
    engine::ui::UiPanel* health_panel_{nullptr};                  // 生命值图标面板;
};
} // scene

#endif //GAME_SCENE_H
