//
// Created by wangm on 26-10-14.
//

#include "scene_manager.h"
#include "../log/my_log.h"
#include "scene.h"

namespace engine {
namespace scene {
SceneManager::SceneManager(engine::core::Context& context)
    : context_(context) {
    mylog::MyLog{}.trace("create SceneManager complete");
}

SceneManager::~SceneManager() {
    close();
    mylog::MyLog{}.trace("delete SceneManager complete");
}

void SceneManager::handelInput() {
    auto current_scene = getCurrentScene();
    if (current_scene) {
        current_scene->handleInput();
    }
}

void SceneManager::update(const float dt) {
    auto scene = getCurrentScene();
    if (scene) {
        scene->update(dt);
    }
    processPendingActions();
}

void SceneManager::render() {
    for (const auto& scene: scenes_stack_) {
        scene->render();
    }
}

void SceneManager::close() {
    if (pending_action_ != PendingAction::None) {
        pending_scene_->clean();
        pending_action_ = PendingAction::None;
    }
}

void SceneManager::requestPushScene(std::unique_ptr<engine::scene::Scene>&& scene) {
    if (!scene) {
        mylog::MyLog{}.warn("requestPushScene failed,scene null");
        return;
    }

    pending_scene_ = std::move(scene);
    pending_action_ = PendingAction::Push;
}

void SceneManager::requestPopScene() {
    pending_action_ = PendingAction::Pop;
}

void SceneManager::requestReplaceScene(std::unique_ptr<engine::scene::Scene>&& scene) {
    if (!scene) {
        mylog::MyLog{}.warn("requestReplaceScene failed,scene null");
        return;
    }

    pending_scene_ = std::move(scene);
    pending_action_ = PendingAction::Replace;
}

engine::scene::Scene* SceneManager::getCurrentScene() const {
    if (scenes_stack_.empty()) { return nullptr; }
    return scenes_stack_.back().get();
}

void SceneManager::processPendingActions() {
    if (pending_action_ == PendingAction::None) { return; }
    switch (pending_action_) {
        case PendingAction::Push:
            pushScene();
            break;
        case PendingAction::Pop:
            popScene();
            break;
        case PendingAction::Replace:
            replaceScene();
            break;
        default:
            break;
    }
    pending_action_ = PendingAction::None;
}

void SceneManager::pushScene() {
    mylog::MyLog{}.trace("pushScene start");
    if (!pending_scene_->isInitialized()) {
        pending_scene_->init();
    }
    scenes_stack_.push_back(std::move(pending_scene_));
}

void SceneManager::popScene() {
    if (scenes_stack_.empty()) { return; }
    scenes_stack_.back()->clean();
    scenes_stack_.pop_back();
}

void SceneManager::replaceScene() {
    // 清空栈
    for (const auto& scene: scenes_stack_) {
        scene->clean();
    }
    scenes_stack_.clear();

    // 新场景初始化
    if (!pending_scene_->isInitialized()) {
        pending_scene_->init();
    }

    // 新创建入栈；
    scenes_stack_.push_back(std::move(pending_scene_));
}
} // scene
} // engine
