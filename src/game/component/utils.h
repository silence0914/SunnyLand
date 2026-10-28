//
// Created by wangm on 26-10-27.
//

#ifndef UTILS_H
#define UTILS_H
#include <array>
#include <string>
#include <string_view>

namespace game::component {
enum class State {
    IDLE = 0, // 待机
    WALK,     // 移动
    JUMP,     // 跳跃
    FALL,     // 下落
    HURT,     // 受伤
    DEAD,     // 死亡
    FLY,      // 飞行
    COUNT
};

static constexpr std::array<std::pair<State, std::string_view>, static_cast<size_t>(State::COUNT)> StateMap{
    {
        {State::IDLE, "idle"},
        {State::WALK, "walk"},
        {State::JUMP, "jump"},
        {State::FALL, "fall"},
        {State::HURT, "hurt"},
        {State::DEAD, "idle"},
        {State::FLY, "fly"},

    }
};

inline std::string toString(State s) {
    for (auto& [k,v]: StateMap) {
        if (k == s) { return std::string{v}; }
    }
    return "";
}
}


#endif //UTILS_H
