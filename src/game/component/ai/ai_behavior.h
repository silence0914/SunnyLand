//
// Created by wangm on 26-10-27.
//

#ifndef AI_BEHAVIOR_H
#define AI_BEHAVIOR_H
#include "../utils.h"

namespace game::component {
class AiComponent;
}

namespace game {
namespace component {
namespace ai {
/**
 * @brief ai 行为抽象类；
 */
class AiBehavior {
    friend class game::component::AiComponent;

public:
    AiBehavior() = default;
    virtual ~AiBehavior() = default;

    AiBehavior(const AiBehavior& other) = delete;
    AiBehavior(AiBehavior&& other) noexcept = delete;
    AiBehavior& operator=(const AiBehavior& other) = delete;
    AiBehavior& operator=(AiBehavior&& other) noexcept = delete;

protected:
    virtual void enter(game::component::AiComponent&) {}
    virtual void update(const float, game::component::AiComponent&) = 0; // 更新ai行为逻辑；
};
} // ai
} // component
} // game

#endif //AI_BEHAVIOR_H
