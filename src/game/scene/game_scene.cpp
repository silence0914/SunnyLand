//
// Created by wangm on 26-10-18.
//

#include "game_scene.h"

#include <utility>

#include "../../engine/audio/audio_player.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/log/log_manager.h"
#include "../../engine/log/my_log.h"
#include "../../engine/physics/aabb_collider.h"
#include "../../engine/physics/circleCollider.h"
#include "../../engine/render/camera.h"
#include "../../engine/render/text_renderer.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"
#include "../../engine/utils/action.h"
#include "../component/ai_component.h"
#include "../component/state/player_state.h"
#include "../component/player_component.h"
#include "../component/ai/patrol_behavior.h"
#include "../component/ai/updown_behavior.h"
#include "../data/session_data.h"

namespace game::scene {
GameScene::GameScene(engine::core::Context& context,
                     engine::scene::SceneManager& scene_manager,
                     std::shared_ptr<game::data::SessionData> data)
    : Scene("GameScene", context, scene_manager), game_session_data_(std::move(data)) {
    if (!game_session_data_) {
        game_session_data_ = std::make_shared<game::data::SessionData>();
    }

    mylog::MyLog{}.trace("create GameScene success");
}

void GameScene::init() {
    if (isInitialized()) { return; }

    if (!initLevel()) {
        mylog::MyLog{}.error("scene init failed;");
        context_.getInputManager().setShouldQuit(true);
        return;
    }

    if (!initPlayer()) {
        mylog::MyLog{}.error("player init failed");
        context_.getInputManager().setShouldQuit(true);
        return;
    }

    if (!initEnemyAndItem()) {
        mylog::MyLog{}.error("enemy and item init failed");
        context_.getInputManager().setShouldQuit(true);
        return;
    }

    if (!initUi()) {
        mylog::MyLog{}.error("ui init failed");
        context_.getInputManager().setShouldQuit(true);
        return;
    }

    // 设置音量
    context_.getAudioPlayer().setMusicVolume(0.2f);
    context_.getAudioPlayer().setSoundVolume(0.5f);

    context_.getAudioPlayer().playMusic("assets/audio/hurry_up_and_run.ogg", -1, 1000);

    Scene::init();
    mylog::MyLog{}.trace("init success");
}

void GameScene::handleInput() {
    Scene::handleInput();
}

void GameScene::update(const float dt) {
    Scene::update(dt);
    handleObjectCollisions();
    handleTileTriggers();
}

void GameScene::render() {
    Scene::render();
}

void GameScene::clean() {
    Scene::clean();
}

bool GameScene::initLevel() {
    // 加载管卡；
    engine::scene::LevelLoader level_loader;
    auto level_path = game_session_data_->getMapPath();
    if (!level_loader.loadLevel(level_path, *this)) {
        mylog::MyLog{}.error("'{}' initLevel failed;loadLevel failed;", scene_name_);
    }

    // 注册“main”层到物理引擎；
    auto* main_layer = findGameObjectByName("main");
    if (!main_layer) {
        mylog::MyLog{}.error("'{}' initLevel failed;findGameObjectByName 'main' failed;", scene_name_);
        return false;
    }

    engine::component::TileLayerComponent* tile_layer = main_layer->getComponent<
        engine::component::TileLayerComponent>();
    if (tile_layer == nullptr) {
        mylog::MyLog{}.error("'{}' initLevel failed;findTileLayerComponent failed;", scene_name_);
        return false;
    }

    // 注册到物理层
    context_.getPhysicsEngine().registerCollisionLayer(*tile_layer);

    auto world_size = tile_layer->getWorldSize();
    context_.getCamera().setLimitBounds(engine::utils::Rect{glm::vec2{0.0f}, world_size});
    spdlog::trace("initLevel success; scene_name = '{}'", scene_name_);
    return true;
}

bool GameScene::initPlayer() {
    // 获取玩家对象
    player_ = findGameObjectByName("player");
    if (!player_) {
        mylog::MyLog{}.error("'{}' initPlayer failed;findGameObjectByName 'player' failed;", scene_name_);
        return false;
    }

    // 添加playerComponent 到玩家对象
    auto* player_component = player_->addComponent<game::component::PlayerComponent>();
    if (!player_component) {
        mylog::MyLog{}.error("'{}' initPlayer failed;addPlayerComponent failed;", scene_name_);
        return false;
    }

    // 相机跟随
    auto* player_transform = player_->getComponent<engine::component::TransformComponent>();
    if (!player_transform) {
        mylog::MyLog{}.error("'{}' initPlayer failed;findTransformComponent failed;", scene_name_);
        return false;
    }
    context_.getCamera().setTarget(*player_transform);
    spdlog::trace("initPlayer success; scene_name = '{}'", scene_name_);
    return true;
}

bool GameScene::initEnemyAndItem() {
    bool success = true;
    for (auto& game_object: game_objects_) {
        if (game_object->getName() == "eagle") {
            if (auto* ai_component = game_object->addComponent<game::component::AiComponent>()) {
                auto y_down = game_object->getComponent<engine::component::TransformComponent>()->getPosition().y;
                auto y_up = y_down - 80.0f; // 鹰巡逻范围为初始位置 至 其上方80像素；
                ai_component->setBehavior(std::make_unique<game::component::ai::UpdownBehavior>(y_up, y_down));
            }
        } else if (game_object->getName() == "frog") {
            if (auto* ai_component = game_object->addComponent<game::component::AiComponent>()) {
                auto x_right = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x;
                auto x_left = x_right - 90.0f;
                ai_component->setBehavior(std::make_unique<game::component::ai::JumpBehavior>(x_left, x_right));
            }
        } else if (game_object->getName() == "opossum") {
            if (auto* ai_component = game_object->addComponent<game::component::AiComponent>()) {
                auto x_right = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x;
                auto x_left = x_right - 200.0f;
                ai_component->setBehavior(std::make_unique<game::component::ai::PatrolBehavior>(x_left, x_right));
            }
        }
        if (game_object->getTag() == "item") {
            if (auto* ac = game_object->getComponent<engine::component::AnimationComponent>(); ac) {
                ac->playAnimation(component::toString(component::State::IDLE));
            } else {
                spdlog::error("Item对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
    }
    return success;
}

bool GameScene::initUi() {
    if (!ui_manager_->init(glm::vec2(context_.getCamera().getViewportSize()))) { return false; }
    createScoreUi();
    createHealthUi();
    createTestButton();
    return true;
}

void GameScene::handleObjectCollisions() {
    auto& collision_pairs = context_.getPhysicsEngine().getCollisionPairs();
    for (auto& collision_pair: collision_pairs) {
        auto* obj1 = collision_pair.first;
        auto* obj2 = collision_pair.second;

        engine::object::GameObject* player{nullptr};
        engine::object::GameObject* other{nullptr};
        if (obj1->getName() == "player") {
            player = obj1;
            other = obj2;
        } else if (obj2->getName() == "player") {
            player = obj2;
            other = obj1;
        }

        if (player) {
            if (other->getTag() == "enemy") {
                playerVsEnemyCollision(*player, *other);
            } else if (other->getTag() == "item") {
                playerVsItemCollision(*player, *other);
            } else if (other->getTag() == "hazard") {
                handlePlayerDamage(1);
            } else if (other->getTag() == "next_level") {
                toNextLevel(other);
            }
        }
    }
}

void GameScene::handleTileTriggers() {
    const auto& tile_trigger_events = context_.getPhysicsEngine().getTileTriggerEvent();
    for (const auto& event: tile_trigger_events) {
        auto* obj = event.first;
        auto tile_type = event.second;
        if (tile_type == engine::component::TileType::HAZARD) {
            if (obj->getName() == "player") { obj->getComponent<game::component::PlayerComponent>()->takeDamage(1); }
        }
    }
}

void GameScene::handlePlayerDamage(int damage) {
    auto player_component = player_->getComponent<game::component::PlayerComponent>();
    if (!player_component->takeDamage(damage)) { return; }
    if (player_component->isDead()) {
        mylog::MyLog{}.info("player dead");
    }
    // 更新ui
    updateHealthWithUi();
}

void GameScene::playerVsEnemyCollision(engine::object::GameObject& player, engine::object::GameObject& enemy) {
    auto player_aabb = player.getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto enemy_aabb = enemy.getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto player_center = player_aabb.getCenter();
    auto enemy_center = enemy_aabb.getCenter();
    auto overlap = glm::vec2(player_aabb.size / 2.0f + enemy_aabb.size / 2.0f) - glm::abs(player_center - enemy_center);

    // 踩踏成功；
    if (overlap.x > overlap.y && player_center.y < enemy_center.y) {
        auto enemy_health = enemy.getComponent<engine::component::HealthComponent>();
        if (!enemy_health) { return; }
        enemy_health->takeDamage(1);
        if (!enemy_health->isAlive()) {
            enemy.setNeedRemove(true);                  // 怪物死亡  标记为待删除
            createEffect(enemy_center, enemy.getTag()); // 创建死亡动画；
        }
        player.getComponent<engine::component::PhysicsComponent>()->setVelocityY(-300.f); // 向上跳起；
        context_.getAudioPlayer().playSound("assets/audio/punch2a.mp3");
        addScoreWithUi(10);
    } else {
        // 踩踏失败； 受到伤害；
        handlePlayerDamage(1);
    }
}

void GameScene::playerVsItemCollision(engine::object::GameObject&, engine::object::GameObject& item) {
    if (item.getName() == "fruit") {
        healWithUi(1); // 回血；
    } else if (item.getName() == "gem") {
        addScoreWithUi(5);
    }
    item.setNeedRemove(true);
    auto item_aabb = item.getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    createEffect(item_aabb.getCenter(), item.getTag());
    context_.getAudioPlayer().playSound("assets/audio/poka01.mp3");
}

void GameScene::toNextLevel(engine::object::GameObject* trigger) {
    auto scene_name = trigger->getName();
    auto map_path = levelNameToPath(scene_name);
    game_session_data_->setNextLevel(map_path);
    auto next_scene = std::make_unique<game::scene::GameScene>(context_, scene_manager_, game_session_data_);
    scene_manager_.requestReplaceScene(std::move(next_scene));
}

void GameScene::createEffect(const glm::vec2& center_pos, const std::string& tag) {
    auto effect_obj = std::make_unique<engine::object::GameObject>("effect_" + tag);
    effect_obj->addComponent<engine::component::TransformComponent>(center_pos);

    // 根据标签创建不同的精灵组件和动画；
    auto animation = std::make_unique<engine::render::Animation>("effect", false);
    if (tag == "enemy") {
        effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/enemy-deadth.png",
                                                                     &context_.getResourceManager(),
                                                                     engine::utils::Alignment::CENTER);
        for (int i = 0; i < 5; ++i) {
            animation->addFrame({static_cast<float>(i * 40), 0.0f, 40.0f, 41.0f}, 0.1f);
        }
    } else if (tag == "item") {
        effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/item-feedback.png",
                                                                     &context_.getResourceManager(),
                                                                     engine::utils::Alignment::CENTER);
        for (int i = 0; i < 5; ++i) {
            animation->addFrame({static_cast<float>(i * 32), 0.0f, 32.0f, 32.0f}, 0.1f);
        }
    } else {
        return;
    }
    auto* animation_component = effect_obj->addComponent<engine::component::AnimationComponent>();
    animation_component->addAnimation(std::move(animation));
    animation_component->setIsOneShotRemoval(true); // 动画结束后删除；
    animation_component->playAnimation("effect");   // 播放动画
    safeAddGameObject(std::move(effect_obj));       // 安全添加特效对象；
}

void GameScene::createScoreUi() {
    auto score_text = "Score: " + std::to_string(game_session_data_->getCurrentScore());
    auto score_label = std::make_unique<engine::ui::UiLabel>(context_.getTextRenderer(), score_text,
                                                             "assets/fonts/VonwaonBitmap-16px.ttf", 16);
    if (!score_label) {
        mylog::MyLog{}.error("Failed to create score label");
        return;
    }
    auto screen_size = ui_manager_->getRootElement()->getSize();
    score_label->setPosition(glm::vec2(screen_size.x - 100.0f, 10.0f));
    score_label_ = score_label.get();
    ui_manager_->addElement(std::move(score_label));
}

void GameScene::createHealthUi() {
    int max_health = game_session_data_->getMaxHealth();
    int current_health = game_session_data_->getCurrentHealth();
    float start_x = 10.0f;
    float start_y = 10.0f;
    float icon_width = 20.0f;
    float icon_height = 18.0f;
    float spacing = 5.0f;
    std::string full_heart_tex = "assets/textures/UI/Heart.png";
    std::string empty_heart_tex = "assets/textures/UI/Heart-bg.png";

    auto health_panel = std::make_unique<engine::ui::UiPanel>();
    health_panel_ = health_panel.get();

    for (int i = 0; i < max_health; ++i) {
        glm::vec2 icon_pos{start_x + i * spacing + i * icon_width, start_y};
        glm::vec2 icon_size{icon_width, icon_height};
        auto bg_icon = std::make_unique<engine::ui::UiImage>(empty_heart_tex, icon_pos, icon_size);
        health_panel_->addChild(std::move(bg_icon));
    }
    for (int i = 0; i < current_health; ++i) {
        glm::vec2 icon_pos{start_x + i * spacing + i * icon_width, start_y};
        glm::vec2 icon_size{icon_width, icon_height};
        auto fg_icon = std::make_unique<engine::ui::UiImage>(full_heart_tex, icon_pos, icon_size);
        health_panel_->addChild(std::move(fg_icon));
    }

    ui_manager_->addElement(std::move(health_panel));
}

void GameScene::addScoreWithUi(int score) {
    game_session_data_->addScore(score);
    score_label_->setText("Score : " + std::to_string(game_session_data_->getCurrentScore()));
}

void GameScene::healWithUi(int amount) {
    player_->getComponent<engine::component::HealthComponent>()->heal(amount);
    updateHealthWithUi();
}

void GameScene::updateHealthWithUi() {
    if (!player_ || !health_panel_) { return; }
    auto player_health_component = player_->getComponent<engine::component::HealthComponent>();
    int current_health = player_health_component->getCurrentHealth();
    auto max_health = player_health_component->getMaxHealth();
    for (auto i = max_health; i < max_health * 2; ++i) {
        health_panel_->getChildren()[i]->setVisible(i - max_health < current_health);
    }
}

void GameScene::createTestButton() {
    auto test_button = std::make_unique<engine::ui::UiButton>(context_, "assets/textures/UI/buttons/Start1.png",
                                                              "assets/textures/UI/buttons/Start2.png",
                                                              "assets/textures/UI/buttons/Start3.png",
                                                              glm::vec2{100.0f, 100.0f},
                                                              glm::vec2{0.0f},
                                                              [this]() { this->testButtonClicked(); });
    ui_manager_->addElement(std::move(test_button));
}

void GameScene::testButtonClicked() {
    spdlog::info("testButtonClicked");
}
} // scene
// game
