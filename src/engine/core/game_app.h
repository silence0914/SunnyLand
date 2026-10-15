//
// Created by wangm on 26-10-8.
//

#ifndef GAME_APP_H
#define GAME_APP_H

#include <memory>
#include <chrono>

struct SDL_Window;
struct SDL_Renderer;

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {
class Camera;
class Renderer;
}

namespace engine::input {
class InputManager;
}

namespace engine::object {
class GameObject;
}

namespace engine::core {
class Time;
class Config;
class Context;

class GameApp final {
public:
    GameApp();
    void Run();
    ~GameApp();

    GameApp(const GameApp&) = delete;
    GameApp(const GameApp&&) = delete;
    GameApp& operator=(GameApp&) = delete;
    GameApp& operator=(GameApp&&) = delete;

private:
    [[nodiscard]] bool init();
    void handleEvents();
    void update(const std::chrono::milliseconds& dt);
    void render();
    void close();

    [[nodiscard]] bool initSDL();
    [[nodiscard]] bool initTime();
    [[nodiscard]] bool initResourceManager();
    [[nodiscard]] bool initRenderer();
    [[nodiscard]] bool initCamera();
    [[nodiscard]] bool initConfig();
    [[nodiscard]] bool initInputManager();
    [[nodiscard]] bool initContext();


    // 测试
    void testResourceManager();
    void testRenderer();
    void testCamera();
    void testInputManager();
    void testGameObject();

private:
    SDL_Window* window_{nullptr};
    SDL_Renderer* sdl_renderer_{nullptr};
    bool is_running_{false};

    std::unique_ptr<engine::core::Time> time_;                            // 时间管理
    std::unique_ptr<engine::resource::ResourceManager> resource_manager_; // 资源管理
    std::unique_ptr<engine::render::Camera> camera_;                      // 摄像头
    std::unique_ptr<engine::render::Renderer> renderer_;                  // 渲染
    std::unique_ptr<engine::core::Config> config_;                        // 配置
    std::unique_ptr<engine::input::InputManager> input_manager_;          // 输入管理
    std::unique_ptr<engine::core::Context> context_;                      // 上下文
};
}


#endif //GAME_APP_H
