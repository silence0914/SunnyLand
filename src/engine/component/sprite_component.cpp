//
// Created by wangm on 26-10-14.
//

#include "sprite_component.h"

#include <spdlog/spdlog.h>

#include "transform_component.h"
#include "../core/context.h"
#include "../object/game_object.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"

namespace engine {
namespace component {
SpriteComponent::SpriteComponent(const std::string& texture_id, engine::resource::ResourceManager* resource_manager,
                                 engine::utils::Alignment alignment, std::optional<SDL_FRect> source_rect,
                                 bool is_flipped)
    : resource_manager_(resource_manager), sprite_{texture_id, source_rect, is_flipped}, alignment_(alignment) {
    if (resource_manager == nullptr) {
        spdlog::critical("SpriteComponent: resource_manager == nullptr;");
    }
    spdlog::trace("create SpriteComponent completion");
}

void SpriteComponent::setSpriteById(const std::string& texture_id, const std::optional<SDL_FRect>& source_rect) {
    sprite_.setTextureId(texture_id);
    sprite_.setSourceRect(source_rect);

    updateOffset();
}

void SpriteComponent::setAlignment(engine::utils::Alignment alignment) {
    alignment_ = alignment;
    updateOffset();
}

void SpriteComponent::updateSpriteSize() {
    // sprite_ 中有source_rect 尺寸  直接使用；
    if (sprite_.getSourceRect().has_value()) {
        sprite_size_ = {sprite_.getSourceRect()->w, sprite_.getSourceRect()->h};
        return;
    }
    if (resource_manager_ == nullptr) {
        if (owner_ == nullptr) {
            spdlog::error("updateSpriteSize failed,SpritComponent owner = nullptr, resource_manager_ == nullptr");
            return;
        }
        spdlog::error("updateSpriteSize failed,SpritComponent '{}' resource_manager_ == nullptr", owner_->getName());
        return;
    }
    // sprite_ 中 source_rect = nullopt， 通过SDL查询；
    sprite_size_ = resource_manager_->getTextureSize(sprite_.getTextureId());
}

void SpriteComponent::init() {
    if (!owner_) {
        spdlog::error("SpriteComponent init failed;owner == nullptr ");
        return;
    }
    transform_component_ = owner_->getComponent<TransformComponent>();
    if (!transform_component_) {
        spdlog::warn("SpriteComponent init failed;owner_->getComponent<TransformComponent> = nullptr");
        return;
    }
    updateSpriteSize();
    updateOffset();
}

void SpriteComponent::render(engine::core::Context& context) {
    if (is_hidden_ || resource_manager_ == nullptr || transform_component_ == nullptr) {
        return;
    }

    // 获取绘制信息
    const glm::vec2& pos = transform_component_->getPosition() + offset_;
    const glm::vec2& scale = transform_component_->getScale();
    float rotation = transform_component_->getRotation();

    // 绘制精灵图
    context.getRenderer().drawSprite(context.getCamera(), sprite_, pos, scale, rotation);
}


void SpriteComponent::updateOffset() {
    // 尺寸非法； 偏移量设为0；
    if (sprite_size_.x < 0 || sprite_size_.y < 0) {
        offset_ = {0.0f, 0.0f};
        return;
    }
    auto scale = transform_component_->getScale();
    auto& x = sprite_size_.x;
    auto& y = sprite_size_.y;

    switch (alignment_) {
        case engine::utils::Alignment::TOP_LEFT: offset_ = {0, 0};
            break;
        case engine::utils::Alignment::TOP_CENTER: offset_ = {x / 2, 0};
            break;
        case engine::utils::Alignment::TOP_RIGHT: offset_ = {x, 0};
            break;
        case engine::utils::Alignment::CENTER_LEFT: offset_ = {0, y / 2};
            break;
        case engine::utils::Alignment::CENTER: offset_ = {x / 2, y / 2};
            break;
        case engine::utils::Alignment::CENTER_RIGHT: offset_ = {x, y / 2};
            break;
        case engine::utils::Alignment::BOTTOM_LEFT: offset_ = {0, y};
            break;
        case engine::utils::Alignment::BOTTOM_CENTER: offset_ = {x / 2, y};
            break;
        case engine::utils::Alignment::BOTTOM_RIGHT: offset_ = {x, y};
            break;
        default:
            break;
    }
}
} // component
} // engine
