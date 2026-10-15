//
// Created by wangm on 26-10-14.
//

#ifndef CONTEXT_H
#define CONTEXT_H

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
    Context(engine::render::Camera& camera, engine::render::Renderer& renderer,
            engine::input::InputManager& input_manager, engine::resource::ResourceManager& resource_manager);
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

private:
    engine::render::Camera& camera_;
    engine::render::Renderer& renderer_;
    engine::input::InputManager& input_manager_;
    engine::resource::ResourceManager& resource_manager_;
};
} // core
// engine

#endif //CONTEXT_H
