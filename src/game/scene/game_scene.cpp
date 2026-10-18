//
// Created by wangm on 26-10-18.
//

#include "game_scene.h"

#include "../../engine/object/game_object.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/core/context.h"
#include "../../engine/log/log_manager.h"
#include "../../engine/log/my_log.h"

namespace game::scene {
GameScene::GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene(name, context, scene_manager) {
    mylog::MyLog{}.trace("create GameScene success");
}

void GameScene::init() {
    Scene::init();
    createTestObject();
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

void GameScene::createTestObject() {
    mylog::MyLog{}.trace("test create object of GameObject");
    auto test_object = std::make_unique<engine::object::GameObject>("test object");

    test_object->addComponent<engine::component::TransformComponent>(glm::vec2{300.0f, 100.0f});
    test_object->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png",
                                                                  &getContext().getResourceManager());

    addGameObject(std::move(test_object));
}
} // scene
// game
