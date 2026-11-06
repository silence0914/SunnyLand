//
// Created by wangm on 26-11-5.
//

#ifndef END_SCENE_H
#define END_SCENE_H
#include "../../engine/scene/scene.h"

namespace game::data {
class SessionData;
}

namespace game {
namespace scene {
class EndScene final : public engine::scene::Scene {
public:
    EndScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager,
             std::shared_ptr<game::data::SessionData>& session_data);

    ~EndScene() override = default;

    void init() override;

    EndScene(const EndScene& other) = delete;
    EndScene(EndScene&& other) noexcept = delete;
    EndScene& operator=(const EndScene& other) = delete;
    EndScene& operator=(EndScene&& other) noexcept = delete;
private:
    void createUi();
    void onBackClick();
    void onRestartClick();

private:
    std::shared_ptr<game::data::SessionData> session_data_;
};
} // scene
} // game

#endif //END_SCENE_H
