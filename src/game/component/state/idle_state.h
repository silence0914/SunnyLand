//
// Created by wangm on 26-10-24.
//

#ifndef IDLE_STATE_H
#define IDLE_STATE_H
#include "player_state.h"

namespace game {
namespace component {
namespace state {
class IdleState final : public game::component::state::PlayerState {
    friend class game::component::PlayerComponent;

public:
    explicit IdleState(PlayerComponent& player_component): PlayerState(player_component) {}
    ~IdleState() override = default;

protected:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context& context) override;
    std::unique_ptr<PlayerState> update(const float dt, engine::core::Context& context) override;
};
} // state
} // component
} // game

#endif //IDLE_STATE_H
