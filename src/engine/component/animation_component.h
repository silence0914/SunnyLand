//
// Created by wangm on 26-10-25.
//

#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H
#include <unordered_map>

#include "component.h"
#include "../render/animation.h"
#include "../render/renderer.h"

namespace engine::component {
class SpriteComponent;
}

namespace engine::render {
class Animation;
}

namespace engine {
namespace component {
/**
 * @brief GameObject 动画组件
 *
 * 持有一组Animation对象 并控制其播放；
 * 根据当前帧更新关联的spriteComponent
 */
class AnimationComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    AnimationComponent() = default;
    ~AnimationComponent() override = default;

    AnimationComponent(const AnimationComponent& other) = delete;
    AnimationComponent(AnimationComponent&& other) noexcept = delete;
    AnimationComponent& operator=(const AnimationComponent& other) = delete;
    AnimationComponent& operator=(AnimationComponent&& other) noexcept = delete;

    void addAnimation(std::unique_ptr<engine::render::Animation> animation); // 添加一个动画；
    void playAnimation(const std::string& name);                             // 播放指定名称的动画；
    void stopAnimation() { is_playing_ = false; }                            // 停止当前动画的播放；

    // get/set
    [[nodiscard]] const std::string& getCurrentAnimationName() const { return current_animation_->getName(); }
    [[nodiscard]] bool isPlaying() const { return is_playing_; }
    [[nodiscard]] bool isAnimationFinished() const;
    [[nodiscard]] bool isOneShotRemoval() const { return is_one_shot_removal_; }
    void setIsOneShotRemoval(bool is_one_shot_removal) { is_one_shot_removal_ = is_one_shot_removal; }

protected:
    void init() override;
    void update(const float, engine::core::Context&) override;

private:
    std::unordered_map<std::string, std::unique_ptr<engine::render::Animation> > animations_; // 动画名称 -> Animation 映射
    engine::component::SpriteComponent* sprite_component_{nullptr};                           // SpriteComponent 视觉组件；
    engine::render::Animation* current_animation_{nullptr};                                   // 当前播放的动画

    float animation_timer_{0.0f};     // 动画播放计时器；
    bool is_playing_{false};          // 当前是否有动画在播放；
    bool is_one_shot_removal_{false}; // 是否在动画结束后删除整个GameObject;
};
} // component
} // engine

#endif //ANIMATION_COMPONENT_H
