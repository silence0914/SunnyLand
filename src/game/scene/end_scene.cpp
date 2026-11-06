//
// Created by wangm on 26-11-5.
//

#include "end_scene.h"

#include "game_scene.h"
#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/log/my_log.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"
#include "../data/session_data.h"

namespace game {
namespace scene {
EndScene::EndScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager,
                   std::shared_ptr<game::data::SessionData>& session_data)
    : Scene("EndScene", context, scene_manager), session_data_(std::move(session_data)) {
    if (!session_data_) {
        mylog::MyLog{}.error("session_data_ = null");
    }
}

void EndScene::init() {
    if (is_initialized_) { return; }
    // 游戏状态设置为game_over;
    context_.getGameState().setState(engine::core::State::GAME_OVER);
    createUi();
    Scene::init();
}

void EndScene::createUi() {
    auto window_size = context_.getGameState().getLogicalSize();
    auto center_x = [window_size](float size) { return (window_size.x - size) / 2; };

    if (!ui_manager_->init(window_size)) {
        mylog::MyLog{}.error("EndScene create ui failed, ui_manager init failed");
        return;
    }
    auto is_win = session_data_->getIsWin();
    std::string main_message = is_win ? "YOU WIN！ CONGRATS!" : "YOU DIED! TRY AGAIN";
    engine::utils::FColor message_color = is_win
                                              ? engine::utils::FColor{0.0f, 1.0f, 0.0f, 1.0f}
                                              : engine::utils::FColor{1.0f, 0.0f, 0.0f, 1.0f};

    auto main_label = std::make_unique<engine::ui::UiLabel>(context_.getTextRenderer(),
                                                            main_message,
                                                            "assets/fonts/VonwaonBitmap-16px.ttf",
                                                            48,
                                                            message_color);

    glm::vec2 label_size = main_label->getSize();
    glm::vec2 main_label_pos = {center_x(label_size.x), window_size.y * 0.3f};
    main_label->setPosition(main_label_pos);
    ui_manager_->addElement(std::move(main_label));

    // 得分标签
    int current_score = session_data_->getCurrentScore();
    int high_score = session_data_->getHighScore();
    engine::utils::FColor score_color = {1.0f, 1.0f, 1.0f, 1.0f};
    int score_font_size = 24;
    std::string score_text = "Score: " + std::to_string(current_score);
    auto score_label = std::make_unique<engine::ui::UiLabel>(context_.getTextRenderer(),
                                                             score_text,
                                                             "assets/fonts/VonwaonBitmap-16px.ttf",
                                                             score_font_size,
                                                             score_color);
    glm::vec2 score_label_size = score_label->getSize();
    glm::vec2 score_label_pos = {center_x(score_label_size.x), main_label_pos.y + label_size.y + 20.0f};
    score_label->setPosition(score_label_pos);
    ui_manager_->addElement(std::move(score_label));

    // 最高分
    std::string high_score_text = "High Score: " + std::to_string(high_score);
    auto high_score_label = std::make_unique<engine::ui::UiLabel>(context_.getTextRenderer(),
                                                                  high_score_text,
                                                                  "assets/fonts/VonwaonBitmap-16px.ttf",
                                                                  score_font_size, score_color);
    glm::vec2 high_score_label_size = high_score_label->getSize();
    glm::vec2 high_score_label_pos = {
        center_x(high_score_label_size.x), score_label_pos.y + score_label_size.y + 10.0f
    };
    high_score_label->setPosition(high_score_label_pos);
    ui_manager_->addElement(std::move(high_score_label));

    // ui按钮
    glm::vec2 button_size = {120.0f, 40.0f};
    float button_spacing = 20.0f;
    float total_button_width = button_size.x * 2 + button_spacing;
    float buttons_x = window_size.x - total_button_width - 30.0f;
    float buttons_y = window_size.y - button_size.y - 30.0f;
    auto back_button = std::make_unique<engine::ui::UiButton>(context_,
                                                              "assets/textures/UI/buttons/Back1.png",
                                                              "assets/textures/UI/buttons/Back2.png",
                                                              "assets/textures/UI/buttons/Back3.png",
                                                              glm::vec2{buttons_x, buttons_y},
                                                              button_size,
                                                              [this]() { this->onBackClick(); });
    ui_manager_->addElement(std::move(back_button));

    buttons_x += button_size.x + button_spacing;
    auto restart = std::make_unique<engine::ui::UiButton>(context_,
                                                          "assets/textures/UI/buttons/Restart1.png",
                                                          "assets/textures/UI/buttons/Restart2.png",
                                                          "assets/textures/UI/buttons/Restart3.png",
                                                          glm::vec2{buttons_x, buttons_y},
                                                          button_size,
                                                          [this]() { this->onRestartClick(); });
    ui_manager_->addElement(std::move(restart));
}

void EndScene::onBackClick() {
    scene_manager_.requestReplaceScene(std::make_unique<TitleScene>(context_, scene_manager_, session_data_));
}

void EndScene::onRestartClick() {
    session_data_.reset();
    scene_manager_.requestReplaceScene(std::make_unique<GameScene>(context_, scene_manager_, session_data_));
}
} // scene
} // game
