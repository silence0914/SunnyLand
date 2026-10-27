//
// Created by wangm on 26-10-14.
//

#include "scene.h"

#include <spdlog/spdlog.h>

#include "../core/context.h"
#include "../object/game_object.h"
#include "../physics/physics_engine.h"
#include "../render/camera.h"

namespace engine {
namespace scene {
Scene::Scene(std::string scene_name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : scene_name_(std::move(scene_name)), context_(context), scene_manager_(scene_manager) {
    spdlog::trace("create scene completion");
}

void Scene::init() {
    is_initialized_ = true;
    spdlog::trace("scene '{}' init completion", scene_name_);
}

void Scene::handleInput() {
    if (!is_initialized_) {
        return;
    }
    for (const auto& game_object: game_objects_) {
        if (game_object->isNeedRemove()) { break; }
        game_object->handleInput(context_);
    }
}

void Scene::update(const float dt) {
    if (!is_initialized_) { return; }

    // 先删除无效对象；
    for (auto it = game_objects_.begin(); it != game_objects_.end();) {
        auto& game_object = *it;
        if (!game_object) {
            it = game_objects_.erase(it);
            break;
        }
        // 删除对象；
        if (game_object->isNeedRemove()) {
            game_object->clear();
            it = game_objects_.erase(it);
            break;
        }
        ++it;
    }


    // 更新物理引擎；
    context_.getPhysicsEngine().update(dt);

    // 更新相机
    context_.getCamera().update(dt);

    for (auto & object : game_objects_) {
        object->update(dt,context_);
    }

    processPendingAdditions();
}

void Scene::render() {
    if (!is_initialized_) { return; }
    for (const auto& game_object: game_objects_) {
        game_object->render(context_);
    }
}

void Scene::clean() {
    if (!is_initialized_) { return; }
    for (const auto& game_object: game_objects_) {
        game_object->clear();
    }
    game_objects_.clear();
    for (const auto& padding_addition: pending_additions) {
        padding_addition->clear();
    }
    pending_additions.clear();
    is_initialized_ = false;
}

void Scene::addGameObject(std::unique_ptr<engine::object::GameObject>&& game_object) {
    if (!game_object) {
        spdlog::warn("addGameObject failed; game_object = nullptr)");
        return;
    }
    game_objects_.emplace_back(std::move(game_object));
}

void Scene::safeAddGameObject(std::unique_ptr<engine::object::GameObject>&& game_object) {
    if (!game_object) {
        spdlog::warn("safeAddGameObject failed; game_object = nullptr)");
        return;
    }
    game_objects_.emplace_back(std::move(game_object));
}

void Scene::removeGameObject(engine::object::GameObject* game_object) {
    if (game_object == nullptr) {
        spdlog::warn("removeGameObject failed, game_object == nullptr");
        return;
    }
    auto it = std::remove_if(game_objects_.begin(), game_objects_.end(),
                             [game_object](const std::unique_ptr<engine::object::GameObject>& p) {
                                 return p.get() == game_object;
                             });
    if (it == game_objects_.end()) {
        spdlog::warn("removeGameObject failed, scene '{}' not find game_object ", scene_name_);
    }
    game_objects_.erase(it, game_objects_.end());
    spdlog::trace("scene '{}' removeGameObject success;", scene_name_);
}

void Scene::safeRemoveGameObject(engine::object::GameObject* game_object) {
    game_object->setNeedRemove(true);
}

engine::object::GameObject* Scene::findGameObjectByName(const std::string& game_object_name) {
    for (const auto& game_object: game_objects_) {
        if (game_object->getName() == game_object_name) {
            return game_object.get();
        }
    }
    spdlog::warn("scene '{}' not findGameObjectByName '{}'", scene_name_, game_object_name);
    return nullptr;
}

void Scene::processPendingAdditions() {
    for (auto& pending_addition: pending_additions) {
        addGameObject(std::move(pending_addition));
    }
    pending_additions.clear();
}
} // scene
} // engine
