//
// Created by wangm on 26-10-24.
//

#ifndef JUMP_STATE_H
#define JUMP_STATE_H
#include "player_state.h"


namespace game {
namespace component {
namespace state {
class JumpState final : public game::component::state::PlayerState {
    friend class game::component::PlayerComponent;

public:
    explicit JumpState(game::component::PlayerComponent& player_component): PlayerState(player_component) {}
    ~JumpState() override = default;

protected:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
    std::unique_ptr<PlayerState> update(const std::chrono::milliseconds&, engine::core::Context&) override;
};
} // state
} // component
} // game

#endif //JUMP_STATE_H
