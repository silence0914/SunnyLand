//
// Created by wangm on 26-11-4.
//

#ifndef TITLE_SCENE_H
#define TITLE_SCENE_H
#include "../../engine/scene/scene.h"

namespace game::data {
class SessionData;
}

namespace game {
namespace scene {
class TitleScene final : public engine::scene::Scene {
public:
    TitleScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager,
               const std::shared_ptr<game::data::SessionData>& session_data = nullptr);

    ~TitleScene() override = default;

    void init() override;
    void update(float dt) override;

    TitleScene(const TitleScene& other) = delete;
    TitleScene(TitleScene&& other) noexcept = delete;
    TitleScene& operator=(const TitleScene& other) = delete;
    TitleScene& operator=(TitleScene&& other) noexcept = delete;

private:
    void createUi();
    void onStartGameClick();
    void onLoadGameClick();
    void onHelpClick();
    void onQuitClick();

private:
    std::shared_ptr<game::data::SessionData> session_data_;
};
} // scene
} // game

#endif //TITLE_SCENE_H
