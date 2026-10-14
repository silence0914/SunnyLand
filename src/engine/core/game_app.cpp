//
// Created by wangm on 26-10-8.
//


#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "time.h"
#include "game_app.h"

#include "config.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"
#include "../resource/audio_manager.h"
#include "../resource/texture_manager.h"
#include "../resource/font_manager.h"
#include "../input/input_manager.h"
#include "../object/game_object.h"

namespace engine::core {
GameApp::GameApp() = default;

void GameApp::Run() {
    if (!init()) {
        spdlog::debug("GameApp init failed");
        return;
    }
    // test resourceManager;
    testResourceManager();

    while (is_running_) {
        time_->update();
        input_manager_->update();
        handleEvents();
        update(time_->getDeltaTime());
        render();
    }
    close();
}

GameApp::~GameApp() {
    if (is_running_) {
        spdlog::warn("关闭流程出现问题， 未显式关闭， 现在关闭");
        close();
    }
}

bool GameApp::init() {
    if (!initConfig()) { return false; }
    if (!initSDL()) { return false; }
    if (!initResourceManager()) { return false; }
    if (!initCamera()) { return false; }
    if (!initRenderer()) { return false; }
    if (!initInputManager()) { return false; }

    if (!initTime()) { return false; }
    is_running_ = true;
    return true;
}

void GameApp::handleEvents() {
    if (input_manager_->shouldQuit()) {
        is_running_ = false;
        return;
    }
    testInputManager();
}

void GameApp::update(const std::chrono::milliseconds& dt) {
    testCamera();
}

void GameApp::render() {
    // 清空屏幕
    renderer_->clearScreen();

    // 测试
    testRenderer();

    // 更新屏幕显示；
    renderer_->present();
}

void GameApp::close() {
    spdlog::trace("game app close");

    // 先释放Audio 后释放 windows; 否则音频退出会出错；
    resource_manager_.reset();

    if (sdl_renderer_ != nullptr) {
        SDL_DestroyRenderer(sdl_renderer_);
        sdl_renderer_ = nullptr;
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
    is_running_ = false;
}

bool GameApp::initSDL() {
    spdlog::trace("sdl 初始化");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        spdlog::error("SDL_Init failed");
        return false;
    }
    window_ = SDL_CreateWindow(config_->window_title_.c_str(), config_->window_width_, config_->window_height_,
                               SDL_WINDOW_RESIZABLE);
    if (window_ == nullptr) {
        spdlog::error("create window failed, error : {}", SDL_GetError());
        return false;
    }
    sdl_renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (sdl_renderer_ == nullptr) {
        spdlog::error("create render failed, error : {}", SDL_GetError());
        return false;
    }

    int vsync_mode = config_->vsync_enabled_ ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED;
    SDL_SetRenderVSync(sdl_renderer_, vsync_mode);

    SDL_SetRenderLogicalPresentation(sdl_renderer_, config_->window_width_ / 2, config_->window_height_ / 2,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    spdlog::trace("initSDL success;");
    return true;
}

bool GameApp::initTime() {
    try {
        time_ = std::make_unique<Time>(config_->target_fps_);
    } catch (const std::exception& e) {
        spdlog::error("initTime failed; error : {}", e.what());
        return false;
    }
    spdlog::trace("initTime success;");
    return true;
}

bool GameApp::initResourceManager() {
    try {
        resource_manager_ = std::make_unique<engine::resource::ResourceManager>(sdl_renderer_);
    } catch (const std::exception& e) {
        spdlog::error("initResourceManager failed;error : {}", e.what());
        return false;
    }
    spdlog::trace("initResourceManager success;");
    return true;
}

bool GameApp::initRenderer() {
    try {
        renderer_ = std::make_unique<engine::render::Renderer>(sdl_renderer_, resource_manager_.get());
    } catch (const std::exception& e) {
        spdlog::error("initRenderer failed;error : {}", e.what());
        return false;
    }
    return true;
}

bool GameApp::initCamera() {
    try {
        camera_ = std::make_unique<engine::render::Camera>(
            glm::vec2(config_->window_width_ / 2, config_->window_height_ / 2));
    } catch (const std::exception& e) {
        spdlog::error("initCamera failed;error : {}", e.what());
        return false;
    }
    return true;
}

bool GameApp::initConfig() {
    try {
        config_ = std::make_unique<engine::core::Config>("assets/config.json");
    } catch (const std::exception& e) {
        spdlog::error("initConfig failed; error : {}", e.what());
        return false;
    }
    return true;
}

bool GameApp::initInputManager() {
    try {
        input_manager_ = std::make_unique<engine::input::InputManager>(sdl_renderer_, config_.get());
    } catch (const std::exception& e) {
        spdlog::error("initInputManager failed; error : {}", e.what());
        return false;
    }
    return true;
}

void GameApp::testResourceManager() {
    resource_manager_->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->getTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->getSound("assets/audio/button_click.wav");

    resource_manager_->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->unloadTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->unloadSound("assets/audio/button_click.wav");
}

void GameApp::testRenderer() {
    engine::render::Sprite sprite_frog("assets/textures/Actors/frog.png");
    engine::render::Sprite sprite_back("assets/textures/Layers/back.png");
    engine::render::Sprite sprite_helps("assets/textures/UI/buttons/Helps1.png");

    static float rotation = 0.0f;
    rotation += 0.1f;

    renderer_->drawParallax(*camera_, sprite_back, glm::vec2{100, 100}, glm::vec2{0.5, 0.5}, glm::bvec2{true, false});
    renderer_->drawSprite(*camera_, sprite_frog, glm::vec2{200, 100}, glm::vec2{1.0f, 1.0f}, rotation);
    renderer_->drawUISprite(sprite_helps, glm::vec2{300, 300});
}

void GameApp::testCamera() {
    auto key_state = SDL_GetKeyboardState(nullptr);
    if (key_state[SDL_SCANCODE_UP]) {
        camera_->move(glm::vec2{0, -1});
    }
    if (key_state[SDL_SCANCODE_DOWN]) {
        camera_->move(glm::vec2{0, 1});
    }
    if (key_state[SDL_SCANCODE_LEFT]) {
        camera_->move(glm::vec2{-1, 0});
    }
    if (key_state[SDL_SCANCODE_RIGHT]) {
        camera_->move(glm::vec2{1, 0});
    }
}

void GameApp::testInputManager() {
    std::vector<std::string> actions = {
        "move_up",
        "move_down",
        "move_left",
        "move_right",
        "jump",
        "attack",
        "pause",
        "MouseLeftClick",
        "MouseRightClick"
    };

    for (const auto& action: actions) {
        if (input_manager_->isActionPressed(action)) {
            spdlog::info(" {} 按下 ", action);
        }
        if (input_manager_->isActionReleased(action)) {
            spdlog::info(" {} 抬起 ", action);
        }
        if (input_manager_->isActionDown(action)) {
            spdlog::info(" {} 按下中 ", action);
        }
    }
}

void GameApp::testGameObject() {
    engine::object::GameObject game_object("test_game_object");
    game_object.addComponent<engine::component::Component>();
}
}
