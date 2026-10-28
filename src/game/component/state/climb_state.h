//
// Created by wangm on 26-10-28.
//

#ifndef CLIMB_STATE_H
#define CLIMB_STATE_H
#include "player_state.h"

namespace game {
namespace component {
namespace state {

class ClimbState final : public game::component::state::PlayerState{
    friend class game::component::PlayerComponent;
public:
    explicit ClimbState(PlayerComponent& player_component)
        : PlayerState(player_component) {}

    ~ClimbState() override = default;

protected:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
    std::unique_ptr<PlayerState> update(const float, engine::core::Context&) override;
};

} // state
} // component
} // game

#endif //CLIMB_STATE_H