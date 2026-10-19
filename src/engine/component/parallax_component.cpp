//
// Created by wangm on 26-10-19.
//

#include "parallax_component.h"

#include <spdlog/spdlog.h>

#include "../log/log_manager.h"
#include "../log/my_log.h"
#include "../object/game_object.h"
#include "transform_component.h"
#include "../core/context.h"
#include "../render/renderer.h"

namespace engine {
namespace component {
ParallaxComponent::ParallaxComponent(const std::string& texture_id, const glm::vec2& scroll_factor,
                                     const glm::bvec2& repeat)
    : sprite_(texture_id), scroll_factor_(scroll_factor), repeat_(repeat) {
    mylog::MyLog{}.trace("create ParallaxComponent success; texture_id = {}", texture_id);
}

void ParallaxComponent::init() {
    if (!owner_) {
        mylog::MyLog{}.error("init failed; owner = nullptr; texture_id = {}", sprite_.getTextureId());
        return;
    }
    transform_ = owner_->getComponent<engine::component::TransformComponent>();
    if (!transform_) {
        spdlog::error("init failed; transform = nullptr;");
        return;
    }
}

void ParallaxComponent::render(engine::core::Context& context) {
    if (is_hidden_ || !transform_) {
        return;
    }
    context.getRenderer().drawParallax(context.getCamera(), sprite_, transform_->getPosition(), scroll_factor_, repeat_,
                                       transform_->getScale());
}
} // component
} // engine
