//
// Created by wangm on 26-10-26.
//

#ifndef HURT_STATE_H
#define HURT_STATE_H
#include "player_state.h"

namespace game {
namespace component {
namespace state {
class HurtState final : public PlayerState {
    friend class game::component::PlayerComponent;

public:
    explicit HurtState(PlayerComponent& player_component): PlayerState(player_component) {}
    ~HurtState() override = default;

protected:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
    std::unique_ptr<PlayerState> update(const float, engine::core::Context&) override;

private:
    float stunned_timer_{0.0f}; // 硬直计时器；
};
} // state
} // component
} // game

#endif //HURT_STATE_H
