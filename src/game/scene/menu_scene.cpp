//
// Created by wangm on 26-11-4.
//

#include "menu_scene.h"

#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/log/my_log.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"
#include "../data/session_data.h"

namespace game {
namespace scene {
MenuScene::MenuScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager,
                     std::shared_ptr<game::data::SessionData> session_data)
    : Scene("MenuScene", context, scene_manager), session_data_(std::move(session_data)) {
    if (!session_data_) {
        mylog::MyLog{}.error("session data is null");
    }
}

void MenuScene::init() {
    context_.getGameState().setState(engine::core::State::PAUSED);
    createUi();
    Scene::init();
}

void MenuScene::handleInput() {
    Scene::handleInput();
    // 暂停时  再次按暂停  恢复游戏
    if (context_.getInputManager().isActionPressed(std::string{engine::utils::action::PAUSE})) {
            scene_manager_.requestPopScene();
            context_.getGameState().setState(engine::core::State::PLAYING);
    }
}

void MenuScene::createUi() {
    auto window_size = context_.getGameState().getLogicalSize();
    if (!ui_manager_->init(window_size)) { return; }

    // 创建标签
    auto pause_label = std::make_unique<engine::ui::UiLabel>(context_.getTextRenderer(), "PAUSE",
                                                             "assets/fonts/VonwaonBitmap-16px.ttf", 32);
    auto size = pause_label->getSize();
    auto label_pos = glm::vec2{(window_size.x - size.x) / 2.0f, window_size.y * 0.2};
    pause_label->setPosition(label_pos);
    ui_manager_->addElement(std::move(pause_label));

    // 创建按钮
    float button_width = 96.0f;
    float button_height = 32.0f;
    float button_spacing = 10.0f;
    glm::vec2 button_size{button_width, button_height};
    float start_y = label_pos.y + 80.0f;
    float button_x = (window_size.x - button_width) / 2.0f;

    auto resume_button = std::make_unique<engine::ui::UiButton>(context_,
                                                                "assets/textures/UI/buttons/Resume1.png",
                                                                "assets/textures/UI/buttons/Resume2.png",
                                                                "assets/textures/UI/buttons/Resume3.png",
                                                                glm::vec2{button_x, start_y},
                                                                button_size,
                                                                [this]() { onResumeClicked(); });
    ui_manager_->addElement(std::move(resume_button));

    start_y += button_height + button_spacing;
    auto save_button = std::make_unique<engine::ui::UiButton>(context_,
                                                              "assets/textures/UI/buttons/Save1.png",
                                                              "assets/textures/UI/buttons/Save2.png",
                                                              "assets/textures/UI/buttons/Save3.png",
                                                              glm::vec2{button_x, start_y},
                                                              button_size,
                                                              [this]() { onSaveClicked(); });
    ui_manager_->addElement(std::move(save_button));

    start_y += button_height + button_spacing;
    auto back_button = std::make_unique<engine::ui::UiButton>(context_,
                                                              "assets/textures/UI/buttons/Back1.png",
                                                              "assets/textures/UI/buttons/Back2.png",
                                                              "assets/textures/UI/buttons/Back3.png",
                                                              glm::vec2{button_x, start_y},
                                                              button_size,
                                                              [this]() { onBackClicked(); });
    ui_manager_->addElement(std::move(back_button));

    start_y += button_height + button_spacing;
    auto quit_button = std::make_unique<engine::ui::UiButton>(context_,
                                                              "assets/textures/UI/buttons/Quit1.png",
                                                              "assets/textures/UI/buttons/Quit2.png",
                                                              "assets/textures/UI/buttons/Quit3.png",
                                                              glm::vec2{button_x, start_y},
                                                              button_size,
                                                              [this]() { onQuitClicked(); });
    ui_manager_->addElement(std::move(quit_button));
}

void MenuScene::onResumeClicked() {
    spdlog::trace("resume button clicked");
    scene_manager_.requestPopScene();
    context_.getGameState().setState(engine::core::State::PLAYING);
}

void MenuScene::onSaveClicked() {
    spdlog::trace("save button clicked");
    if (session_data_ && session_data_->saveToFile(game::data::DEFAULT_SAVE_PATH)) {
        mylog::MyLog{}.info("save to file success");
    } else {
        mylog::MyLog{}.error("save to file failed");
    }
}

void MenuScene::onBackClicked() {
    spdlog::trace("back button clicked");
    scene_manager_.requestReplaceScene(std::make_unique<game::scene::TitleScene>(context_, scene_manager_,
                                                                                 session_data_));
}

void MenuScene::onQuitClicked() {
    spdlog::trace("quit button clicked");
    context_.getInputManager().setShouldQuit(true);
}
} // scene
} // game
