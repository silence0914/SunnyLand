//
// Created by wangm on 26-10-20.
//

#include "tilelayer_component.h"


#include "../core/context.h"
#include "../log/log_manager.h"
#include "../log/my_log.h"
#include "../physics/physics_engine.h"
#include "../render/renderer.h"

namespace engine {
namespace component {
TileLayerComponent::TileLayerComponent(glm::ivec2 tile_size, glm::ivec2 map_size, std::vector<TileInfo>&& tiles)
    : tile_size_(tile_size), map_size_(map_size), tiles_(std::move(tiles)) {
    if (tiles_.size() != static_cast<size_t>(map_size_.x * map_size_.y)) {
        mylog::MyLog().error("TileLayerComponent: tiles size not match map size");
        tiles_.clear();
        map_size_ = {0, 0};
    }
    mylog::MyLog{}.trace("TileLayerComponent success");
}

const TileInfo* TileLayerComponent::getTileInfoAt(glm::ivec2 pos) const {
    if (pos.x < 0 || pos.y < 0) {
        mylog::MyLog{}.warn("TileLayerComponent; pos.x or pos.y < 0");
        return nullptr;
    }
    size_t index = static_cast<size_t>(pos.y * map_size_.x + pos.x);
    if (index > tiles_.size()) {
        mylog::MyLog{}.warn("TileLayerComponent; index out of range");
        return nullptr;
    }
    return &tiles_[index];
}

TileType TileLayerComponent::getTileTypeAt(glm::ivec2 pos) const {
    const auto& info = getTileInfoAt(pos);
    return info ? info->type : TileType::EMPTY;
}

TileType TileLayerComponent::getTileTypeAtWorldPos(glm::vec2 world_pos) const {
    glm::vec2 relative_pos = world_pos - offset_;
    int x = static_cast<int>(std::floor(world_pos.x / tile_size_.x));
    int y = static_cast<int>(std::floor(world_pos.y / tile_size_.y));
    return getTileTypeAt({x, y});
}

void TileLayerComponent::init() {
    if (!owner_) {
        mylog::MyLog{}.warn("TileLayerComponent; owner_ = nullptr");
    }
    mylog::MyLog{}.trace("TileLayerComponent init success");
}

void TileLayerComponent::render(engine::core::Context& context) {
    if (tile_size_.x <= 0 || tile_size_.y <= 0) {
        return;
    }
    for (int y = 0; y < map_size_.y; ++y) {
        for (int x = 0; x < map_size_.x; ++x) {
            size_t index = static_cast<size_t>(y) * map_size_.x + x;
            if (index < tiles_.size() && tiles_[index].type != TileType::EMPTY) {
                const auto& tile_info = tiles_[index];
                glm::vec2 tile_pos = {
                    offset_.x + static_cast<float>(x * tile_size_.x),
                    offset_.y + static_cast<float>(y * tile_size_.y)
                };

                if (static_cast<int>(tile_info.sprite.getSourceRect()->h) != tile_size_.y) {
                    tile_pos.y -= tile_info.sprite.getSourceRect()->h - static_cast<float>(tile_size_.y);
                }
                context.getRenderer().drawSprite(context.getCamera(), tile_info.sprite, tile_pos);
            }
        }
    }
}

void TileLayerComponent::clear() {
    // physics_engine_->unregisterComponent(*this);
}
} // component
} // engine
