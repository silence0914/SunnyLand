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
#include "../component/state/player_state.h"

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

    Scene::init();
    mylog::MyLog{}.trace("init success");
}

void GameScene::handleInput() {
    Scene::handleInput();
}

void GameScene::update(const std::chrono::milliseconds& dt) {
    Scene::update(dt);
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
} // scene
// game
