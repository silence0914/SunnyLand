//
// Created by wangm on 26-10-20.
//

#ifndef TILELAYER_COMPONENT_H
#define TILELAYER_COMPONENT_H
#include <glm/vec2.hpp>

#include "component.h"
#include "../render/sprite.h"


namespace engine::physics {
class PhysicsEngine;
}

namespace engine {
namespace component {
enum class TileType {
    EMPTY,     // 空白瓦片
    NORMAL,    // 普通瓦片
    SOLID,     // 静止可碰撞瓦片
    UNISOLID,  // 单向静止可碰撞瓦片
    SLOPE_0_1, // 斜坡瓦片 左0 右1
    SLOPE_0_2, // 斜坡瓦片 左0 右2
    SLOPE_1_0, // 斜坡瓦片 左1 右0
    SLOPE_1_2, // 斜坡瓦片 左1 右2
    SLOPE_2_0, // 斜坡瓦片 左2 右0
    SLOPE_2_1, // 斜坡瓦片 左2 右1
};

struct TileInfo {
    engine::render::Sprite sprite; // 瓦片素材对象
    TileType type;                 // 瓦片类型；
    explicit TileInfo(engine::render::Sprite s = engine::render::Sprite(),
                      TileType T = TileType::EMPTY) : sprite(std::move(s)), type(T) {};
};

/**
 * @brief 管理渲染瓦片地图层
 */
class TileLayerComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    /**
     * @brief
     * @param tile_size 单个瓦片的尺寸
     * @param map_size 地图尺寸
     * @param tiles 瓦片数据
     */
    TileLayerComponent(glm::ivec2 tile_size, glm::ivec2 map_size, std::vector<TileInfo>&& tiles);
    ~TileLayerComponent() override = default;

    /**
     * @brief 根据瓦片坐标获取瓦片信息
     * @param pos 瓦片坐标
     * @return 指向瓦片信息的 const 指针；无效返回nullptr;
     */
    [[nodiscard]] const TileInfo* getTileInfoAt(glm::ivec2 pos) const;

    /**
     * @brief 根据瓦片坐标获取瓦片类型
     * @param pos 瓦片坐标
     * @return 瓦片类型；无效坐标返回 TileType::EMPTY
     */
    [[nodiscard]] TileType getTileTypeAt(glm::ivec2 pos) const;

    /**
     * @brief 根据世界坐标获取瓦片类型
     * @param world_pos 世界坐标；
     * @return 瓦片类型；无效坐标返回 TileType::EMPTY
     */
    [[nodiscard]] TileType getTileTypeAtWorldPos(glm::vec2 world_pos) const;

    // get/set
    [[nodiscard]] glm::ivec2 getTileSize() const { return tile_size_; }             // 获取瓦片尺寸
    [[nodiscard]] glm::ivec2 getMapSize() const { return map_size_; }               // 获取地图尺寸
    [[nodiscard]] glm::vec2 getWorldSize() const { return map_size_ * tile_size_; } // 获取世界尺寸
    [[nodiscard]] std::vector<TileInfo> getTiles() const { return tiles_; }         // 获取瓦片容器
    [[nodiscard]] glm::vec2 getOffset() const { return offset_; }                   // 获取瓦片层偏移量
    [[nodiscard]] bool isHidden() const { return is_hidden_; }                      // 获取瓦片层是否隐藏

    void setOffset(const glm::vec2& offset) { this->offset_ = offset; } // 设置偏移量
    void setIsHidden(bool is_hidden) { is_hidden_ = is_hidden; }        // 设置是否隐藏；
    void setPhysicsEngine(engine::physics::PhysicsEngine& physics_engine) { physics_engine_ = &physics_engine; }

protected:
    void init() override;
    void update(const float, engine::core::Context&) override {};
    void render(engine::core::Context& context) override;
    void clear() override;

private:
    glm::ivec2 tile_size_;                                    // 单个瓦片的尺寸
    glm::ivec2 map_size_;                                     // 地图尺寸
    std::vector<TileInfo> tiles_;                             // 储存所有瓦片信息
    glm::vec2 offset_{0.0f, 0.0f};                            // 瓦片层在场景中的偏移量；
    bool is_hidden_{false};                                   // 是否隐藏
    engine::physics::PhysicsEngine* physics_engine_{nullptr}; // 物理引擎指针；
};
} // component
} // engine

#endif //TILELAYER_COMPONENT_H
