//
// Created by wangm on 26-10-24.
//

#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

#include <memory>

#include "../player_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/physics/physics_engine.h"


namespace game::component::state {
/**
 * @brief 玩家状态机的基类；
 */
class PlayerState {
    friend class game::component::PlayerComponent;

public:
    explicit PlayerState(PlayerComponent& player_component): player_component_(player_component) {}
    virtual ~PlayerState() = default;

protected:
    virtual void enter() = 0; // 进入；
    virtual void exit() = 0;  // 离开；

    // handleInput 和 update 返回值为下一个状态； 如果不需要切换状态， 返回nullptr;
    virtual std::unique_ptr<PlayerState> handleInput(engine::core::Context&) = 0;                              // 处理输入；
    virtual std::unique_ptr<PlayerState> update(const float, engine::core::Context&) = 0; // 更新；

    void moveLeft() const {
        auto& physics_component = player_component_.getPhysicsComponent();
        if (physics_component.getVelocity().x > 0) {
            // 正在向右移动；
            physics_component.setVelocityX(0.0f);
        }
        physics_component.addForce({-player_component_.getMoveForce(), 0.0f});
        player_component_.getSpriteComponent().setIsFlipped(true);
    }

    void moveRight() const {
        auto& physics_component = player_component_.getPhysicsComponent();
        if (physics_component.getVelocity().x < 0) {
            // 正在向右移动；
            physics_component.setVelocityX(0.0f);
        }
        physics_component.addForce({player_component_.getMoveForce(), 0.0f});
        player_component_.getSpriteComponent().setIsFlipped(false);
    }

protected:
    game::component::PlayerComponent& player_component_; // 拥有此状态机的玩家组件；
};
} // state
// component
// game

#endif //PLAYER_STATE_H
