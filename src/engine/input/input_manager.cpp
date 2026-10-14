//
// Created by wangm on 26-10-13.
//

#include "input_manager.h"
#include "../core/config.h"

#include <spdlog/spdlog.h>
#include <stdexcept>
#include <SDL3/SDL_render.h>

namespace engine {
namespace input {
InputManager::InputManager(SDL_Renderer* renderer, const engine::core::Config* config)
    : sdl_renderer_(renderer) {
    if (sdl_renderer_ == nullptr) {
        spdlog::error("create InputManager failed; sdl_renderer = nullptr");
        throw std::runtime_error("create InputManager failed,sdl = nullptr");
    }
    // 初始化映射表；
    initializeMappings(config);

    // 初始化鼠标坐标；
    SDL_GetMouseState(&mouse_position_.x, &mouse_position_.y);
    spdlog::trace("create InputManager completion");
}

void InputManager::update() {
    // 根据上一帧的状态，更新默认的动作状态；
    for (auto& [action,state]: action_states_) {
        if (state == ActionState::INACTIVE) {
            break;
        }
        if (state == ActionState::PRESSED_THIS_FRAME) {
            state = ActionState::HELD_DOWN;
            break;
        }
        if (state == ActionState::RELEASED_THIS_FRAME) {
            state = ActionState::INACTIVE;
        }
    }

    // 处理所有待处理的 SDL 事件；
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        processEvent(event);
    }
}

bool InputManager::isActionDown(const std::string& action_name) const {
    auto it = action_states_.find(action_name);
    if (it == action_states_.end()) {
        spdlog::warn("isActionDown, action '{}' not exit", action_name);
        return false;
    }
    return it->second == ActionState::HELD_DOWN || it->second == ActionState::PRESSED_THIS_FRAME;
}

bool InputManager::isActionPressed(const std::string& action_name) const {
    auto it = action_states_.find(action_name);
    if (it == action_states_.end()) {
        spdlog::warn("isActionPressed, action '{}' not exit", action_name);
        return false;
    }
    return it->second == ActionState::PRESSED_THIS_FRAME;
}

bool InputManager::isActionReleased(const std::string& action_name) const {
    auto it = action_states_.find(action_name);
    if (it == action_states_.end()) {
        spdlog::warn("isActionReleased, action '{}' not exit", action_name);
        return false;
    }
    return it->second == ActionState::HELD_DOWN;
}

bool InputManager::shouldQuit() const {
    return should_quit_;
}

void InputManager::setShouldQuit(bool shouldQuit) {
    should_quit_ = shouldQuit;
}

glm::vec2 InputManager::getMousePosition() const {
    return mouse_position_;
}

glm::vec2 InputManager::getLogicalMousePosition() const {
    glm::vec2 logical_pos;
    SDL_RenderCoordinatesFromWindow(sdl_renderer_,
                                    mouse_position_.x, mouse_position_.y,
                                    &logical_pos.x, &logical_pos.y);
    return logical_pos;
}

void InputManager::processEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            SDL_Scancode scancode = event.key.scancode;
            bool is_down = event.key.down;
            bool is_repeat = event.key.repeat;

            auto it = input_to_actions_map_.find(scancode);
            if (it == input_to_actions_map_.end()) {
                return;
            }
            const std::vector<std::string>& associated_actions = it->second;
            for (const auto& actions_name: associated_actions) {
                updateActionState(actions_name, is_down, is_repeat);
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            Uint32 button = event.button.button;
            bool is_down = event.button.down;
            auto it = input_to_actions_map_.find(button);
            if (it == input_to_actions_map_.end()) {
                return;
            }
            const std::vector<std::string>& associated_actions = it->second;
            for (const auto& actions_name: associated_actions) {
                updateActionState(actions_name, is_down, false);
            }
            mouse_position_ = {event.button.x, event.button.y};
            break;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            mouse_position_ = {event.button.x, event.button.y};
            break;
        }
        case SDL_EVENT_QUIT: {
            should_quit_ = true;
        }
        default: {
            break;
        }
    }
}

void InputManager::initializeMappings(const engine::core::Config* config) {
    if (config == nullptr) {
        spdlog::error("initializeMappings failed;config = nullptr");
        throw std::runtime_error{"initializeMappings failed;config = nullptr"};
    }
    actions_to_keyname_map_ = config->input_mappings_;
    input_to_actions_map_.clear();
    action_states_.clear();

    if (actions_to_keyname_map_.find("MouseLeftClick") == actions_to_keyname_map_.end()) {
        actions_to_keyname_map_["MouseLeftClick"] = {"MouseLeft"};
    }
    if (actions_to_keyname_map_.find("MouseRightClick") == actions_to_keyname_map_.end()) {
        actions_to_keyname_map_["MouseRightClick"] = {"MouseRight"};
    }

    for (const auto& [action_name,key_names]: actions_to_keyname_map_) {
        action_states_[action_name] = ActionState::INACTIVE;
        for (const std::string& key_name: key_names) {
            SDL_Scancode scancode = scancodeFromString(key_name);
            Uint32 mouse_button = mouseButtonFromString(key_name);

            if (scancode != SDL_SCANCODE_UNKNOWN) {
                input_to_actions_map_[scancode].push_back(action_name);
                spdlog::trace("scancode_to_actions_map; key mapping; {} : {}", static_cast<int>(scancode), action_name);
            } else if (mouse_button != 0) {
                input_to_actions_map_[mouse_button].push_back(action_name);
                spdlog::trace("mouse_button_to_actions_map; key mapping; {} : {}", mouse_button, action_name);
            } else {
                spdlog::warn("key mapping failed;{} : {}", key_name, action_name);
            }
        }
    }
    spdlog::trace("initializeMappings completion");
}

void InputManager::updateActionState(const std::string& action_name, bool is_input_active, bool is_repeat_event) {
    auto it = action_states_.find(action_name);
    if (it == action_states_.end()) {
        spdlog::warn("updateActionState failed; action '{}' not exit;", action_name);
        return;
    }
    if (is_input_active) {
        if (is_repeat_event) {
            it->second = ActionState::HELD_DOWN;
        } else {
            it->second = ActionState::PRESSED_THIS_FRAME;
        }
    } else {
        it->second = ActionState::RELEASED_THIS_FRAME;
    }
}

SDL_Scancode InputManager::scancodeFromString(const std::string& key_name) {
    return SDL_GetScancodeFromName(key_name.c_str());
}

Uint32 InputManager::mouseButtonFromString(const std::string& button_name) {
    if (button_name == "MouseLeft") return SDL_BUTTON_LEFT;
    if (button_name == "MouseMiddle") return SDL_BUTTON_MIDDLE;
    if (button_name == "MouseRight") return SDL_BUTTON_RIGHT;
    if (button_name == "MouseX1") return SDL_BUTTON_X1;
    if (button_name == "MouseX2") return SDL_BUTTON_X2;
    return 0;
}
} // input
} // engine
