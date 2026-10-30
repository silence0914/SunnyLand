//
// Created by wangm on 26-10-18.
//

#include "game_scene.h"

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
#include "../../engine/scene/level_loader.h"
#include "../../engine/utils/action.h"
#include "../component/ai_component.h"
#include "../component/state/player_state.h"
#include "../component/player_component.h"
#include "../component/ai/patrol_behavior.h"
#include "../component/ai/updown_behavior.h"

namespace game::scene {
GameScene::GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene(name, context, scene_manager) {
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
        render();
    }

    if (!initEnemyAndItem()) {
        mylog::MyLog{}.error("enemy and item init failed");
        context_.getInputManager().setShouldQuit(true);
        render();
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
    if (!level_loader.loadLevel("assets/maps/level1.tmj", *this)) {
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

void GameScene::handleObjectCollisions() {
    auto& collision_pairs = context_.getPhysicsEngine().getCollisionPairs();
    for (auto& collision_pair: collision_pairs) {
        auto* obj1 = collision_pair.first;
        auto* obj2 = collision_pair.second;

        if (obj1->getName() == "player") {
            if (obj2->getTag() == "enemy") {
                playerVsEnemyCollision(*obj1, *obj2);
            } else if (obj2->getTag() == "item") {
                playerVsItemCollision(*obj1, *obj2);
            } else if (obj2->getTag() == "hazard") {
                obj1->getComponent<game::component::PlayerComponent>()->takeDamage(1);
            }
        } else if (obj2->getName() == "player") {
            if (obj1->getTag() == "enemy") {
                playerVsEnemyCollision(*obj2, *obj1);
            } else if (obj1->getTag() == "item") {
                playerVsItemCollision(*obj2, *obj1);
            } else if (obj1->getTag() == "hazard") {
                obj2->getComponent<game::component::PlayerComponent>()->takeDamage(1);
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
    } else {
        // 踩踏失败； 受到伤害；
        player.getComponent<game::component::PlayerComponent>()->takeDamage(1);
    }
}

void GameScene::playerVsItemCollision(engine::object::GameObject& player, engine::object::GameObject& item) {
    if (item.getName() == "fruit") {
        player.getComponent<engine::component::HealthComponent>()->heal(1); // 回血；
    } else if (item.getName() == "gem") {}
    item.setNeedRemove(true);
    auto item_aabb = item.getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    createEffect(item_aabb.getCenter(), item.getTag());
    context_.getAudioPlayer().playSound("assets/audio/poka01.mp3");
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
} // scene
// game
