//
// Created by wangm on 26-11-4.
//

#include "title_scene.h"

#include <spdlog/spdlog.h>

#include "game_scene.h"
#include "helps_scene.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/log/my_log.h"
#include "../../engine/render/camera.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../data/session_data.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"

namespace game {
namespace scene {
TitleScene::TitleScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager,
                       const std::shared_ptr<game::data::SessionData>& session_data)
    : Scene("TitleScene", context, scene_manager), session_data_(std::move(session_data)) {
    if (!session_data_) {
        mylog::MyLog{}.warn("session_data_ is null");
        session_data_ = std::make_shared<game::data::SessionData>();
    }
}

void TitleScene::init() {
    if (is_initialized_) { return; }
    engine::scene::LevelLoader level_loader;
    if (!level_loader.loadLevel("assets/maps/level0.tmj", *this)) {
        mylog::MyLog{}.error("Failed to load level");
        return;
    }
    createUi();
    Scene::init();
}

void TitleScene::update(float dt) {
    Scene::update(dt);
    context_.getCamera().move(glm::vec2(dt * 100.0f, 0.0f));
}

void TitleScene::createUi() {
    auto window_size = context_.getCamera().getViewportSize();
    if (!ui_manager_->init(window_size)) {
        spdlog::error("Failed to init ui manager");
        return;
    }
    // 设置音量；
    context_.getAudioPlayer().setMusicVolume(0.2f);
    context_.getAudioPlayer().setSoundVolume(0.5f);

    // 创建标题图片；
    auto title_image = std::make_unique<engine::ui::UiImage>("assets/textures/UI/title-screen.png");
    auto size = context_.getResourceManager().getTextureSize(title_image->getTextureId());
    title_image->setSize(size * 2.0f);

    auto title_pos = (window_size - title_image->getSize()) / 2.0f - glm::vec2{0.0f, 50.0f};
    title_image->setPosition(title_pos);
    ui_manager_->addElement(std::move(title_image));

    // 创建按钮
    float button_width = 96.0f;
    float button_height = 32.0f;
    float button_spacing = 20.0f;
    int num_buttons = 4;

    // 计算面板大小
    glm::vec2 panel_size = {num_buttons * button_width + (num_buttons - 1) * button_spacing, button_height};
    glm::vec2 panel_position = {(window_size.x - panel_size.x) / 2, window_size.y * 0.65};
    auto button_panel = std::make_unique<engine::ui::UiPanel>(panel_position, panel_size);
    glm::vec2 current_button_pos = glm::vec2(0.0f, 0.0f);
    glm::vec2 button_size = glm::vec2{button_width, button_height};

    // State Button;
    auto state_button = std::make_unique<engine::ui::UiButton>(context_,
                                                               "assets/textures/UI/buttons/Start1.png",
                                                               "assets/textures/UI/buttons/Start2.png",
                                                               "assets/textures/UI/buttons/Start3.png",
                                                               current_button_pos,
                                                               button_size,
                                                               [this]() { this->onStartGameClick(); });
    button_panel->addChild(std::move(state_button));

    // load Button
    current_button_pos.x += button_width + button_spacing;
    auto load_button = std::make_unique<engine::ui::UiButton>(context_,
                                                              "assets/textures/UI/buttons/Load1.png",
                                                              "assets/textures/UI/buttons/Load2.png",
                                                              "assets/textures/UI/buttons/Load3.png",
                                                              current_button_pos,
                                                              button_size,
                                                              [this]() { this->onLoadGameClick(); });
    button_panel->addChild(std::move(load_button));

    // helps button
    current_button_pos.x += button_width + button_spacing;
    auto helps_button = std::make_unique<engine::ui::UiButton>(context_,
                                                               "assets/textures/UI/buttons/Helps1.png",
                                                               "assets/textures/UI/buttons/Helps2.png",
                                                               "assets/textures/UI/buttons/Helps3.png",
                                                               current_button_pos,
                                                               button_size,
                                                               [this]() { this->onHelpClick(); });
    button_panel->addChild(std::move(helps_button));

    // quit button
    current_button_pos.x += button_width + button_spacing;
    auto quit_button = std::make_unique<engine::ui::UiButton>(context_,
                                                              "assets/textures/UI/buttons/Quit1.png",
                                                              "assets/textures/UI/buttons/Quit2.png",
                                                              "assets/textures/UI/buttons/Quit3.png",
                                                              current_button_pos,
                                                              button_size,
                                                              [this]() { this->onHelpClick(); });
    button_panel->addChild(std::move(quit_button));
    // ui_panel 添加入ui管理器；
    ui_manager_->addElement(std::move(button_panel));
    // 创建credits 标签；
    auto credits_label = std::make_unique<engine::ui::UiLabel>(context_.getTextRenderer(),
                                                               "SunnyLand Credits: XXX - 2026",
                                                               "assets/fonts/VonwaonBitmap-16px.ttf", 16,
                                                               engine::utils::FColor{0.8f, 0.8f, 0.8f, 1.0f});
    credits_label->setPosition(glm::vec2{
                                   (window_size.x - credits_label->getSize().x) / 2,
                                   window_size.y - credits_label->getSize().y - 10.0f
                               });
    ui_manager_->addElement(std::move(credits_label));
}

void TitleScene::onStartGameClick() {
    if (session_data_) {
        session_data_.reset();
    }
    scene_manager_.requestReplaceScene(std::make_unique<GameScene>(context_, scene_manager_, session_data_));
}

void TitleScene::onLoadGameClick() {
    if (!session_data_) {
        mylog::MyLog{}.error("No session data to load game");
        return;
    }
    if (session_data_->loadFromFile("assets/save.json")) {
        scene_manager_.requestReplaceScene(std::make_unique<GameScene>(context_, scene_manager_, session_data_));
    } else {
        mylog::MyLog{}.warn("Failed to load game from file");
    }
}

void TitleScene::onHelpClick() {
    scene_manager_.requestPushScene(std::make_unique<game::scene::HelpsScene>(context_, scene_manager_));
}

void TitleScene::onQuitClick() {
    context_.getInputManager().setShouldQuit(true);
}
} // scene
} // game
