//
// Created by wangm on 26-10-14.
//

#ifndef CONTEXT_H
#define CONTEXT_H

namespace engine::audio {
class AudioPlayer;
}

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::input {
class InputManager;
}

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {
class Renderer;
class Camera;
}

namespace engine::core {
/**
 * @brief 上下文管理类；
 *
 * 持有核心引擎模块的上下文对象，简化依赖注入；
 */
class Context final {
public:
    Context(engine::render::Camera& camera,
            engine::render::Renderer& renderer,
            engine::input::InputManager& input_manager,
            engine::resource::ResourceManager& resource_manager,
            engine::physics::PhysicsEngine&,
            engine::audio::AudioPlayer&
    );

    ~Context() = default;

    Context(const Context& other) = delete;
    Context(Context&& other) noexcept = delete;
    Context& operator=(const Context& other) = delete;
    Context& operator=(Context&& other) noexcept = delete;

    // get
    [[nodiscard]] engine::render::Camera& getCamera() const { return camera_; }
    [[nodiscard]] engine::render::Renderer& getRenderer() const { return renderer_; }
    [[nodiscard]] engine::input::InputManager& getInputManager() const { return input_manager_; }
    [[nodiscard]] engine::resource::ResourceManager& getResourceManager() const { return resource_manager_; }
    [[nodiscard]] engine::physics::PhysicsEngine& getPhysicsEngine() const { return physics_engine_; }
    [[nodiscard]] engine::audio::AudioPlayer& getAudioPlayer() const { return audio_player_; }

private:
    engine::render::Camera& camera_;
    engine::render::Renderer& renderer_;
    engine::input::InputManager& input_manager_;
    engine::resource::ResourceManager& resource_manager_;
    engine::physics::PhysicsEngine& physics_engine_;
    engine::audio::AudioPlayer& audio_player_;
};
} // core
// engine

#endif //CONTEXT_H
