//
// Created by wangm on 26-10-13.
//

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <unordered_map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <SDL3/SDL_render.h>
#include <variant>
struct SDL_Renderer;

namespace engine::core {
class Config;
}

namespace engine {
namespace input {

enum class ActionState {
    INACTIVE, // 未激活
    PRESSED_THIS_FRAME, // 按键按下
    HELD_DOWN, // 持续按下
    RELEASED_THIS_FRAME // 按键抬起
};

/**
 * @brief 输入管理类
 *
 * 处理鼠标键盘输入事件；
 */
class InputManager final {
public:
    InputManager(SDL_Renderer* renderer, const engine::core::Config* config);
    void update();

    bool isActionDown(const std::string& action_name) const; // 查询 动作是否触发
    bool isActionPressed(const std::string& action_name) const; // 查询 动作是否刚刚按下
    bool isActionReleased(const std::string& action_name) const; // 查询 动作是否刚刚释放

    bool shouldQuit() const; // 查询退出状态；
    void setShouldQuit(bool shouldQuit); // 设置退出状态

    glm::vec2 getMousePosition() const; // 获取鼠标位置 （屏幕坐标）
    glm::vec2 getLogicalMousePosition() const; // 获取鼠标位置 （逻辑坐标）

private:
    SDL_Renderer* sdl_renderer_{nullptr};
    // 动作 -》 按键名称
    std::unordered_map<std::string, std::vector<std::string> > actions_to_keyname_map_;

    // 按键 -》 动作
    std::unordered_map<std::variant<SDL_Scancode, Uint32>, std::vector<std::string> > input_to_actions_map_;

    // 储存动作状态；
    std::unordered_map<std::string, ActionState> action_states_;

    bool should_quit_ = false;
    glm::vec2 mouse_position_{};

private:
    void processEvent(const SDL_Event& event); // 按键信息转换为动作状态；
    void initializeMappings(const engine::core::Config* config); // 初始化按键映射

    void updateActionState(const std::string& action_name, bool is_input_active, bool is_repeat_event); // 更新动作状态；
    SDL_Scancode scancodeFromString(const std::string& key_name); // 将字符串转换成 SDL_Scancode;
    Uint32 mouseButtonFromString(const std::string& button_name); // 将字符串按键命转换为SDL_Button;
};
} // input
} // engine

#endif //INPUT_MANAGER_H
