//
// Created by wangm on 26-10-19.
//

#ifndef ACTION_H
#define ACTION_H
#include <string_view>

namespace engine::utils::action {
constexpr  std::string_view MOVE_UP = "move_up";
constexpr  std::string_view MOVE_DOWN = "move_down";
constexpr  std::string_view MOVE_LEFT = "move_left";
constexpr  std::string_view MOVE_RIGHT = "move_right";
constexpr  std::string_view JUMP = "jump";
constexpr  std::string_view ATTACK = "attack";
constexpr  std::string_view PAUSE = "pause";

}

#endif //ACTION_H
