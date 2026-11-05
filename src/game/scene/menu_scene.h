//
// Created by wangm on 26-11-4.
//

#ifndef MENU_SCENE_H
#define MENU_SCENE_H
#include "../../engine/scene/scene.h"

namespace game::data {
class SessionData;
}

namespace game {
namespace scene {
class MenuScene final : public engine::scene::Scene {
public:
    MenuScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager,
              std::shared_ptr<game::data::SessionData> session_data);

    ~MenuScene() override = default;
    void init() override;
    void handleInput() override;

private:
    void createUi();
    void onResumeClicked();
    void onSaveClicked();
    void onBackClicked();
    void onQuitClicked();

private:
    std::shared_ptr<game::data::SessionData> session_data_;
};
} // scene
} // game

#endif //MENU_SCENE_H
