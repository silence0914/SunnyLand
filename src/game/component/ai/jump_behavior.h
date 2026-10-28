//
// Created by wangm on 26-10-27.
//

#ifndef JUMP_BEHAVIOR_H
#define JUMP_BEHAVIOR_H
#include <glm/vec2.hpp>

#include "ai_behavior.h"

namespace game {
namespace component {
namespace ai {
class JumpBehavior final : public AiBehavior {
    friend class game::component::AiComponent;

public:
    JumpBehavior(float min_x, float max_x, const glm::vec2& jump_vel = {100.0f, -300.0f}, float jump_interval = 2.0f);

    ~JumpBehavior() override = default;

protected:
    void enter(game::component::AiComponent&) override;
    void update(const float, game::component::AiComponent&) override;

private:
    float patrol_min_x_{0.0f};            // 巡逻范围；    左边界
    float patrol_max_x_{0.0f};            // 巡逻范围；    右边界
    glm::vec2 jump_vel_{100.0f, -300.0f}; // 跳跃速度；
    float jump_interval_{2.0f};           // 跳跃间隔事件；
    float jump_timer_{0.0f};              // 距离下次跳跃计时器；
    bool jumping_right_{false};           // 当前是否向右跳跃
};
} // ai
} // component
} // game

#endif //JUMP_BEHAVIOR_H
