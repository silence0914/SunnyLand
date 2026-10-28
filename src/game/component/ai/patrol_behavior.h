//
// Created by wangm on 26-10-27.
//

#ifndef PATROL_BEHAVIOR_H
#define PATROL_BEHAVIOR_H
#include "ai_behavior.h"

namespace game {
namespace component {
namespace ai {
/**
 * @brief 指定范围巡逻；
 *
 * 遇到边界折返；
 */
class PatrolBehavior final : public AiBehavior {
    friend class game::component::AiComponent;

public:
    PatrolBehavior(float patrol_left, float patrol_right, float move_speed = 50.0);
    ~PatrolBehavior() override = default;

protected:
    void enter(game::component::AiComponent&) override;
    void update(const float, game::component::AiComponent&) override;

private:
    float patrol_left_{0.0f};  // 巡逻左边界
    float patrol_right_{0.0f}; // 巡逻右边界
    float move_speed_{50.0};   // 移动速度；
    bool moving_right_{false};  // 是否向右移动；
};
} // ai
} // component
} // game

#endif //PATROL_BEHAVIOR_H
