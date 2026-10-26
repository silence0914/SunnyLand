//
// Created by wangm on 26-10-25.
//

#include "animation_component.h"
#include "../log/my_log.h"
#include "../object/game_object.h"
#include "sprite_component.h"

namespace engine {
namespace component {
void AnimationComponent::addAnimation(std::unique_ptr<engine::render::Animation> animation) {
    if (!animation) {
        mylog::MyLog{}.warn("AnimationComponent::addAnimation() animation is nullptr");
        return;
    }
    animations_[animation->getName()] = std::move(animation);
}

void AnimationComponent::playAnimation(const std::string& name) {
    auto it = animations_.find(name);
    if (it == animations_.end()) {
        mylog::MyLog{}.warn("AnimationComponent::playAnimation() animation {} not found", name);
        return;
    }

    // 如果已经在播放相同动画了  不重新开始；
    if (current_animation_ == it->second.get() && is_playing_) { return; }
    current_animation_ = it->second.get();
    animation_timer_ = 0.0f;
    is_playing_ = true;
    if (!sprite_component_ || current_animation_->isEmpty()) { return; }
    const auto& first_frame = current_animation_->getFrame(0.0f);
    sprite_component_->setSourceRect(first_frame.source_rect);
}

bool AnimationComponent::isAnimationFinished() const {
    if (!current_animation_ || current_animation_->isLoop()) {
        return false;
    }
    return animation_timer_ >= current_animation_->getTotalDuration();
}

void AnimationComponent::init() {
    if (!owner_) {
        mylog::MyLog{}.error("AnimationComponent::~AnimationComponent() owner_ is nullptr");
        return;
    }
    sprite_component_ = owner_->getComponent<engine::component::SpriteComponent>();
    if (!sprite_component_) {
        mylog::MyLog{}.error("AnimationComponent::~AnimationComponent() sprite_component_ is nullptr");
        return;
    }
}

void AnimationComponent::update(const float x, engine::core::Context&) {
    if (!is_playing_ || !current_animation_ || !sprite_component_ || current_animation_->isEmpty()) { return; }

    // 推进播放计时器
    animation_timer_ += x;
    const auto& current_frame = current_animation_->getFrame(animation_timer_);

    // 更新精灵组件的原矩形；
    sprite_component_->setSourceRect(current_frame.source_rect);

    // 检查非循环动画是否已经结束；
    if (!current_animation_->isLoop() && animation_timer_ > current_animation_->getTotalDuration()) {
        is_playing_ = false;
        if (is_one_shot_removal_) {
            owner_->setNeedRemove(true);
        }
    }
}
} // component
} // engine
