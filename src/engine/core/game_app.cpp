//
// Created by wangm on 26-10-8.
//

#include "game_app.h"
#include "time.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "../resource/resource_manager.h"
#include "../resource/audio_manager.h"
#include "../resource/texture_manager.h"
#include "../resource/font_manager.h"

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
    if (!initSDL()) { return false; }
    if (!initResourceManager()) { return false; }
    if (!initTime()) { return false; }
    is_running_ = true;
    return true;
}

void GameApp::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            is_running_ = false;
        }
    }
}

void GameApp::update(const std::chrono::milliseconds& dt) {
}

void GameApp::render() {
}

void GameApp::close() {
    spdlog::trace("game app close");
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
    window_ = SDL_CreateWindow("SunnyLand", 1280, 720,SDL_WINDOW_RESIZABLE);
    if (window_ == nullptr) {
        spdlog::error("create window failed, error : {}", SDL_GetError());
        return false;
    }
    sdl_renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (sdl_renderer_ == nullptr) {
        spdlog::error("create render failed, error : {}", SDL_GetError());
        return false;
    }
    spdlog::trace("initSDL success;");
    return true;
}

bool GameApp::initTime() {
    try {
        time_ = std::make_unique<Time>(60);
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

void GameApp::testResourceManager() {
    resource_manager_->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->getTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->getSound("assets/audio/button_click.wav");

    resource_manager_->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->unloadTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->unloadSound("assets/audio/button_click.wav");
}
}
