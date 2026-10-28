//
// Created by wangm on 26-10-28.
//

#ifndef UPDOWN_BEHAVIOR_H
#define UPDOWN_BEHAVIOR_H
#include "ai_behavior.h"

namespace game {
namespace component {
namespace ai {
class UpdownBehavior final : public game::component::ai::AiBehavior {
    friend class game::component::AiComponent;

public:
    UpdownBehavior(float patrol_up, float patrol_down, float move_speed = 50.0f);
    ~UpdownBehavior() override = default;
    UpdownBehavior(const UpdownBehavior& other) = delete;
    UpdownBehavior(UpdownBehavior&& other) noexcept = delete;
    UpdownBehavior& operator=(const UpdownBehavior& other) = delete;
    UpdownBehavior& operator=(UpdownBehavior&& other) noexcept = delete;

protected:
    void enter(game::component::AiComponent&) override;
    void update(const float, game::component::AiComponent&) override;

private:
    float patrol_up_{0.0f};     // 巡逻上边界
    float patrol_down_{0.0f};   // 巡逻下边界
    float move_speed_{50.0f};  // 移动速度
    bool moving_down_ = false; // 是否向下移动；
};
} // ai
} // component
} // game

#endif //UPDOWN_BEHAVIOR_H
