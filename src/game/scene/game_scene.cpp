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

namespace game::scene {
GameScene::GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene(name, context, scene_manager) {
    mylog::MyLog{}.trace("create GameScene success");
}

void GameScene::init() {
    // 加载关卡
    engine::scene::LevelLoader level_loader;
    level_loader.loadLevel("assets/maps/level1.tmj", *this);

    createTestObject();

    Scene::init();
    mylog::MyLog{}.trace("init success");
}

void GameScene::handleInput() {
    Scene::handleInput();
    testCamera();
    testObject();
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

void GameScene::createTestObject() {
    mylog::MyLog{}.trace("test create object of GameObject");
    auto test_object = std::make_unique<engine::object::GameObject>("test object");
    test_object_ = test_object.get();

    test_object->addComponent<engine::component::TransformComponent>(glm::vec2{100.0f, 100.0f});
    test_object->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png",
                                                                  &getContext().getResourceManager());
    test_object->addComponent<engine::component::PhysicsComponent>(context_.getPhysicsEngine(), false);
    test_object->addComponent<engine::component::ColliderComponent>
            (std::make_unique<engine::physics::AABBCollider>(glm::vec2{32.0f, 32.0f}));

    addGameObject(std::move(test_object));

    auto test_object2 = std::make_unique<engine::object::GameObject>("test_object2");
    test_object2->addComponent<engine::component::TransformComponent>(glm::vec2(50.0f, 50.0f));
    test_object2->addComponent<engine::component::SpriteComponent>
            ("assets/textures/Props/big-crate.png", &context_.getResourceManager());
    test_object2->addComponent<engine::component::PhysicsComponent>(context_.getPhysicsEngine(), false);
    test_object2->addComponent<engine::component::ColliderComponent>
            (std::make_unique<engine::physics::AABBCollider>(glm::vec2{32.0f, 32.0f}));
    addGameObject(std::move(test_object2));
}

void GameScene::testCamera() {
    auto& camera = context_.getCamera();
    auto& input_manager = context_.getInputManager();
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_UP))) { camera.move({0, -1}); }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_DOWN))) { camera.move({0, 1}); }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_LEFT))) { camera.move({-1, 0}); }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_RIGHT))) { camera.move({1, 0}); }
}

void GameScene::testObject() {
    if (!test_object_) { return; }
    auto& input_manager = context_.getInputManager();
    // if (input_manager.isActionDown(std::string(engine::utils::action::JUMP))) {
    //     test_object_->getComponent<engine::component::TransformComponent>()->translate({400, 1});
    // }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_UP))) {
        test_object_->getComponent<engine::component::TransformComponent>()->translate({0, -1});
    }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_DOWN))) {
        test_object_->getComponent<engine::component::TransformComponent>()->translate({0, 1});
    }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_LEFT))) {
        test_object_->getComponent<engine::component::TransformComponent>()->translate({-1, 0});
    }
    if (input_manager.isActionDown(std::string(engine::utils::action::MOVE_RIGHT))) {
        test_object_->getComponent<engine::component::TransformComponent>()->translate({1, 0});
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
