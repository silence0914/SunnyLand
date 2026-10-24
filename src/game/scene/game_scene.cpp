//
// Created by wangm on 26-10-18.
//

#include "game_scene.h"

#include "../../engine/component/collider_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/log/log_manager.h"
#include "../../engine/log/my_log.h"
#include "../../engine/physics/aabb_collider.h"
#include "../../engine/physics/circleCollider.h"
#include "../../engine/render/camera.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/utils/action.h"
#include "../../engine/component/tilelayer_component.h"

namespace game::scene {
GameScene::GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene(name, context, scene_manager) {
    mylog::MyLog{}.trace("create GameScene success");
}

void GameScene::init() {
    // 加载关卡
    engine::scene::LevelLoader level_loader;
    level_loader.loadLevel("assets/maps/level1.tmj", *this);

    // 注册“main”层到物理引擎
    auto main_layer = findGameObjectByName("main");
    if (main_layer) {
        auto* tile_layer = main_layer->getComponent<engine::component::TileLayerComponent>();
        if (tile_layer) {
            context_.getPhysicsEngine().registerCollisionLayer(*tile_layer);
        }
    }

    // 获取玩家对象
    player_ = findGameObjectByName("player");
    if (!player_) {
        mylog::MyLog{}.error("player == nullptr");
        return;
    }

    // 相机更碎玩家
    auto* player_transform = player_->getComponent<engine::component::TransformComponent>();
    if (player_transform) {
        context_.getCamera().setTarget(*player_transform);
    }

    // 设置相机边界
    auto world_size = main_layer->getComponent<engine::component::TileLayerComponent>()->getWorldSize();
    context_.getCamera().setLimitBounds(engine::utils::Rect{glm::vec2{0.0f}, world_size});

    // 设置世界边界；
    context_.getPhysicsEngine().setWorldBounds(engine::utils::Rect{glm::vec2{0.0f}, world_size});

    Scene::init();
    mylog::MyLog{}.trace("init success");
}

void GameScene::handleInput() {
    Scene::handleInput();
    testPlayer();
}

void GameScene::update(const std::chrono::milliseconds& dt) {
    Scene::update(dt);
    testCollisionPairs();
}

void GameScene::render() {
    Scene::render();
}

void GameScene::clean() {
    Scene::clean();
}

// ———————————————————————————— test ——————————————————————————//


void GameScene::testCamera() {
    auto& camera = context_.getCamera();
    auto& input_manager = context_.getInputManager();
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_UP))) { camera.move({0, -1}); }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_DOWN))) { camera.move({0, 1}); }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_LEFT))) { camera.move({-1, 0}); }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_RIGHT))) { camera.move({1, 0}); }
}

void GameScene::testPlayer() {
    if (!player_) { return; }
    auto& input_manager = context_.getInputManager();
    auto* pc = player_->getComponent<engine::component::PhysicsComponent>();
    if (!pc) return;

    if (input_manager.isActionPressed(std::string(engine::utils::action::JUMP))) {
        if (0.1f < pc->getVelocity().y < 0.1f) {
            pc->setVelocityY(-300.0f);
        }
    }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_LEFT))) {
        pc->setVelocityX(-100.0f);
    } else {
        pc->setVelocityX(pc->getVelocity().x * 0.9f);
    }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_RIGHT))) {
        pc->setVelocityX(100.0f);
    } else {
        pc->setVelocityX(pc->getVelocity().x * 0.9f);
    }
}

void GameScene::testCollisionPairs() {
    auto collision_pairs = context_.getPhysicsEngine().getCollisionPairs();
    for (auto& pair: collision_pairs) {
        spdlog::info("'{}' and '{}' collied", pair.first->getName(), pair.second->getName());
    }
}
} // scene
// game
