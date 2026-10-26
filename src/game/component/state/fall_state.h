//
// Created by wangm on 26-10-25.
//

#ifndef FALL_STATE_H
#define FALL_STATE_H
#include "player_state.h"

namespace game {
namespace component {
namespace state {
class FallState final : public game::component::state::PlayerState {
public:
    explicit FallState(PlayerComponent& player_component) : PlayerState(player_component) {}
    ~FallState() override = default;

protected:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
    std::unique_ptr<PlayerState> update(const float, engine::core::Context&) override;
};
} // state
} // component
} // game

#endif //FALL_STATE_H
