//
// Created by wangm on 26-10-26.
//

#ifndef DEAD_STATE_H
#define DEAD_STATE_H
#include "player_state.h"

namespace game {
namespace component {
namespace state {
class DeadState final : public game::component::state::PlayerState {
    friend class game::component::PlayerComponent;

public:
    explicit DeadState(PlayerComponent& player_component): PlayerState(player_component) {}
    ~DeadState() override = default;

protected:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
    std::unique_ptr<PlayerState> update(const float, engine::core::Context&) override;
};
} // state
} // component
} // game

#endif //DEAD_STATE_H
