//
// Created by wangm on 26-10-24.
//

#ifndef WALK_STATE_H
#define WALK_STATE_H
#include "player_state.h"

namespace game {
namespace component {
namespace state {
class WalkState final : public game::component::state::PlayerState {
    friend class game::component::PlayerComponent;

public:
    explicit WalkState(game::component::PlayerComponent& player_component): PlayerState(player_component) {};
    ~WalkState() override = default;

protected:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
    std::unique_ptr<PlayerState> update(const float, engine::core::Context&) override;
};
} // state
} // component
} // game

#endif //WALK_STATE_H
