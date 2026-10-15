//
// Created by wangm on 26-10-14.
//

#include "transform_component.h"

#include "sprite_component.h"
#include "../object/game_object.h"

namespace engine {
namespace component {
void TransformComponent::setScale(const glm::vec2& scale) {
    scale_ = scale;
    if (owner_) {
        auto sprite_component = owner_->getComponent<SpriteComponent>();
        if (sprite_component) {
            sprite_component->updateOffset();
        }
    }
}
} // component
} // engine
