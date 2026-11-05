//
// Created by wangm on 26-11-4.
//

#ifndef GAME_STATE_H
#define GAME_STATE_H
#include <glm/vec2.hpp>
struct SDL_Window;
struct SDL_Renderer;

namespace engine {
namespace core {
/**
 * @brief 当前游戏所属的状态
 */
enum class State {
    TITLE,     // 标题界面；
    PLAYING,   // 游戏进行中
    PAUSED,    // 游戏暂停
    GAME_OVER, // 游戏结束界面
};

class GameState final {
public:
    GameState(SDL_Window* window, SDL_Renderer* renderer, State current_state = State::TITLE);

    ~GameState() = default;

    State getCurrentState() const { return current_state_; }
    glm::vec2 getWindowSize() const;
    glm::vec2 getLogicalSize() const;

    void setState(State new_state) { current_state_ = new_state; }
    void setWindowSize(glm::vec2 new_size);
    void setLogicalSize(glm::vec2 new_size);

    bool isTitle() const { return current_state_ == State::TITLE; }
    bool isPlaying() const { return current_state_ == State::PLAYING; }
    bool isPaused() const { return current_state_ == State::PAUSED; }
    bool isGameOver() const { return current_state_ == State::GAME_OVER; }

private:
    SDL_Window* window_{nullptr};
    SDL_Renderer* renderer_{nullptr};
    State current_state_{State::TITLE}; // 当前游戏状态
};
} // core
} // engine

#endif //GAME_STATE_H
