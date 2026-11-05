//
// Created by wangm on 26-11-4.
//

#include "game_state.h"

#include <glm/vec2.hpp>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "../log/my_log.h"

namespace engine {
namespace core {
GameState::GameState(SDL_Window* window, SDL_Renderer* renderer, State current_state)
    : window_(window), renderer_(renderer), current_state_(current_state) {
    if (!window_ || !renderer_) {
        mylog::MyLog{}.error("window or renderer is null");
        throw std::runtime_error("window or renderer is null");
    }
}

glm::vec2 GameState::getWindowSize() const {
    int width, height;
    SDL_GetWindowSize(window_, &width, &height);
    return glm::vec2{width, height};
}

glm::vec2 GameState::getLogicalSize() const {
    int width, height;
    SDL_GetRenderLogicalPresentation(renderer_, &width, &height,NULL);
    return glm::vec2{width, height};
}

void GameState::setWindowSize(glm::vec2 new_size) {
    SDL_SetWindowSize(window_, static_cast<int>(new_size.x), static_cast<int>(new_size.y));
}

void GameState::setLogicalSize(glm::vec2 new_size) {
    SDL_SetRenderLogicalPresentation(renderer_, static_cast<int>(new_size.x), static_cast<int>(new_size.y),
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
}
} // core
} // engine
