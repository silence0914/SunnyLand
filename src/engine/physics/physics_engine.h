//
// Created by wangm on 26-10-21.
//

#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H
#include <glm/vec2.hpp>
#include "../component/component.h"
#include "../utils/action.h"
#include "../utils/math.h"

namespace engine::component {
class PhysicsComponent;
class TileLayerComponent;
enum class TileType;
}

namespace engine {
namespace physics {
/**
 * @brief 负责 管理和模拟 物理行为和碰撞检测
 */
class PhysicsEngine {
public:
    PhysicsEngine() = default;
    ~PhysicsEngine() = default;

    PhysicsEngine(const PhysicsEngine& other) = delete;
    PhysicsEngine(PhysicsEngine&& other) noexcept = delete;
    PhysicsEngine& operator=(const PhysicsEngine& other) = delete;
    PhysicsEngine& operator=(PhysicsEngine&& other) noexcept = delete;

    void registerComponent(engine::component::PhysicsComponent& component);   // 注册物理组件
    void unregisterComponent(engine::component::PhysicsComponent& component); // 注销物理组件

    void registerCollisionLayer(engine::component::TileLayerComponent& layer);   // 注册用于碰撞检测的TileLayerComponent;
    void unregisterCollisionLayer(engine::component::TileLayerComponent& layer); // 注销用于碰撞检测的TileLayerComponent;


    void update(float dt); // 核心循环， 更新所有注册的物理组件

    // get/set
    [[nodiscard]] glm::vec2 getGravity() const { return gravity_; }                                   // 获取当前全局重力
    [[nodiscard]] float getMaxSpeed() const { return max_speed_; }                                    // 获取当前最大速度
    [[nodiscard]] std::optional<engine::utils::Rect> getWorldBounds() const { return world_bounds_; } // 获取世界边界

    void setGravity(const glm::vec2& gravity) { gravity_ = gravity; } // 设置全局重力
    void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }     // 设置最大速度
    // 设置世界边界
    void setWorldBounds(const std::optional<engine::utils::Rect>& world_bounds) { world_bounds_ = world_bounds; }

    [[nodiscard]] const std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*> >&
    getCollisionPairs() const {
        return collision_pairs_;
    }

private:
    void checkObjectCollisions();                                                  // 检测对象碰撞 并记录；
    void resolveTileCollisions(engine::component::PhysicsComponent& pc, float dt); // 检测并处理游戏对象与瓦片间的碰撞；
    void resolveSolidObjectCollision(engine::object::GameObject& mov_obj, engine::object::GameObject& solid_obj);
    void applyWorldBounds(engine::component::PhysicsComponent& pc); // 应用世界边界  限制物体移动范围；
    /**
     * @brief 根据瓦片类型和指定宽度x坐标；计算瓦片上对应的y坐标；
     * @param width 从瓦片左侧算起的宽度
     * @param type 瓦片类型
     * @param tile_size 瓦片尺寸
     * @return
     */
    float getTileHeightWidth(float width, engine::component::TileType type, glm::vec2 tile_size);

private:
    std::vector<engine::component::PhysicsComponent*> components_;              // 注册的物理组件容器 非拥有指针；
    std::vector<engine::component::TileLayerComponent*> collision_tile_layers_; // 注册的碰撞瓦片图层容器；
    std::optional<engine::utils::Rect> world_bounds_;                           // 世界边界 用于限制物体移动范围；
    glm::vec2 gravity_ = {0.0f, 980.0f};                                        // 默认重力， 像素/秒^2
    float max_speed_ = 500.0f;                                                  // 最大速度 像素/秒；

    // 储存本帧发生的GameObject碰撞对；每次update开始清空；
    std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*> > collision_pairs_;
};
} // physics
} // engine

#endif //PHYSICS_ENGINE_H
