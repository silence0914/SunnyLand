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

namespace engine::core {
class Time;

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

    bool initSDL();
    bool initTime();
    bool initResourceManager();

    void testResourceManager();
private:
    SDL_Window* window_{nullptr};
    SDL_Renderer* sdl_renderer_{nullptr};
    bool is_running_{false};
    std::unique_ptr<engine::core::Time> time_;
    std::unique_ptr<engine::resource::ResourceManager> resource_manager_;
};
}


#endif //GAME_APP_H
