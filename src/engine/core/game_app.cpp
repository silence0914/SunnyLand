//
// Created by wangm on 26-10-8.
//


#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "time.h"
#include "game_app.h"

#include "config.h"
#include "context.h"
#include "../../game/scene/game_scene.h"
#include "../component/sprite_component.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"
#include "../resource/audio_manager.h"
#include "../resource/texture_manager.h"
#include "../resource/font_manager.h"
#include "../input/input_manager.h"
#include "../object/game_object.h"
#include "../component/transform_component.h"
#include "../physics/physics_engine.h"
#include "../scene/scene.h"
#include "../scene/scene_manager.h"

namespace engine::core {
GameApp::GameApp() = default;

void GameApp::Run() {
    if (!init()) {
        spdlog::debug("GameApp init failed");
        return;
    }

    while (is_running_) {
        time_->update();
        input_manager_->update();
        handleEvents();
        update(std::chrono::duration<float>(time_->getDeltaTime()).count());
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
    if (!initSceneManager()) { return false; }
    if (!initPhysicsEngine()) { return false; }

    if (!initContext()) { return false; }
    auto scene = std::make_unique<game::scene::GameScene>("GameScene", *context_, *scene_manager_);
    scene_manager_->requestPushScene(std::move(scene));

    if (!initTime()) { return false; }
    is_running_ = true;
    spdlog::info("gameApp init completion");

    return true;
}

void GameApp::handleEvents() {
    if (input_manager_->shouldQuit()) {
        is_running_ = false;
        return;
    }
    scene_manager_->handelInput();
}

void GameApp::update(const float dt) {
    scene_manager_->update(dt);
}

void GameApp::render() {
    // 清空屏幕
    renderer_->clearScreen();

    // 测试
    scene_manager_->render();
    renderer_->setDrawColor(0,0,255);
    renderer_->drawRect(*camera_,physics_engine_->playerIntersectTile.position,physics_engine_->playerIntersectTile.size);

    // 更新屏幕显示；
    renderer_->present();
}

void GameApp::close() {
    spdlog::trace("game app close");

    // 先关闭场景管理器 确保所有场景都被清理
    scene_manager_->close();

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
                                                           glm::vec2(config_->window_width_ / 2,
                                                                     config_->window_height_ / 2));
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

bool GameApp::initContext() {
    try {
        context_ = std::make_unique<engine::core::Context>(*camera_, *renderer_, *input_manager_, *resource_manager_,
                                                           *physics_engine_);
    } catch (const std::exception& e) {
        spdlog::error("initContext failed; error : {}", e.what());
        return false;
    }
    return true;
}

bool GameApp::initSceneManager() {
    try {
        scene_manager_ = std::make_unique<engine::scene::SceneManager>(*context_);
    } catch (const std::exception& e) {
        spdlog::error("initSceneManager failed;error : {}", e.what());
        return false;
    }
    return true;
}

bool GameApp::initPhysicsEngine() {
    try {
        physics_engine_ = std::make_unique<engine::physics::PhysicsEngine>();
    } catch (const std::exception& e) {
        spdlog::error("initPhysicsEngine failed;error : {}", e.what());
        return false;
    }
    return true;
}
}
