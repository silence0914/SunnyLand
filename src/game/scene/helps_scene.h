//
// Created by wangm on 26-11-4.
//

#ifndef HELPS_SCENE_H
#define HELPS_SCENE_H
#include "../../engine/scene/scene.h"

namespace game {
namespace scene {

class HelpsScene final : public engine::scene::Scene{
public:
    HelpsScene( engine::core::Context& context,engine::scene::SceneManager& scene_manager);

    ~HelpsScene() override = default;
    void init() override;
    void handleInput() override;

    HelpsScene(const HelpsScene& other) = delete;
    HelpsScene(HelpsScene&& other) noexcept = delete;
    HelpsScene& operator=(const HelpsScene& other) = delete;
    HelpsScene& operator=(HelpsScene&& other) noexcept = delete;
};

} // scene
} // game

#endif //HELPS_SCENE_H