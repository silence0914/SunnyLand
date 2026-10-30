//
// Created by wangm on 26-10-26.
//

#include "dead_state.h"

#include "../player_component.h"
#include "../../../engine/component/audio_component.h"
#include "../../../engine/component/collider_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/log/log_manager.h"
#include "../../../engine/log/my_log.h"
#include "../../../engine/object/game_object.h"

namespace game {
namespace component {
namespace state {
void DeadState::enter() {
    mylog::MyLog{}.error("player is dead");
    playAnimation("hurt");

    // 向上击退
    auto& physics_component = player_component_.getPhysicsComponent();
    physics_component.setVelocity({0.0f, -200.0f});

    auto* collider_component = player_component_.getOwner()->getComponent<engine::component::ColliderComponent>();
    if (collider_component) {
        collider_component->setActive(false);
    }

    player_component_.getAudioComponent().playSound("dead");
}

void DeadState::exit() {}

std::unique_ptr<PlayerState> DeadState::handleInput(engine::core::Context&) {
    return nullptr;
}

std::unique_ptr<PlayerState> DeadState::update(const float, engine::core::Context&) {
    return nullptr;
}
} // state
} // component
} // game
