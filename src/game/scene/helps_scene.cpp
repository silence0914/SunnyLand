//
// Created by wangm on 26-11-4.
//

#include "helps_scene.h"

#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_manager.h"

namespace game {
namespace scene {
HelpsScene::HelpsScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene("HelpsScene", context, scene_manager) {}

void HelpsScene::init() {
    if (isInitialized()) { return; }
    auto windows_size = context_.getCamera().getViewportSize();
    auto help_image = std::make_unique<engine::ui::UiImage>("assets/textures/UI/instructions.png",
                                                            glm::vec2{0}, windows_size);
    ui_manager_->addElement(std::move(help_image));
    Scene::init();
}

void HelpsScene::handleInput() {
    if (!isInitialized()){return;}
    if (context_.getInputManager().isActionPressed(std::string{engine::utils::action::MOUSE_LEFT_CLICK})) {
        scene_manager_.requestPopScene();
    }

    Scene::handleInput();
}
} // scene
} // game
