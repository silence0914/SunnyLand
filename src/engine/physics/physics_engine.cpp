//
// Created by wangm on 26-10-21.
//

#include "physics_engine.h"

#include "../component/physics_component.h"
#include <chrono>

#include "collision.h"
#include "../component/collider_component.h"
#include "../component/tilelayer_component.h"
#include "../component/transform_component.h"
#include "../log/my_log.h"
#include "../object/game_object.h"

constexpr std::string_view SOLID{"solid"};

namespace engine {
namespace physics {
void PhysicsEngine::registerComponent(engine::component::PhysicsComponent& component) {
    components_.push_back(&component);
}

void PhysicsEngine::unregisterComponent(engine::component::PhysicsComponent& component) {
    auto it = std::remove(components_.begin(), components_.end(), &component);
    components_.erase(it, components_.end());
}

void PhysicsEngine::registerCollisionLayer(engine::component::TileLayerComponent& layer) {
    layer.setPhysicsEngine(*this);
    collision_tile_layers_.push_back(&layer);
    mylog::MyLog{}.trace("register collision layer success");
}

void PhysicsEngine::unregisterCollisionLayer(engine::component::TileLayerComponent& layer) {
    auto it = std::remove(collision_tile_layers_.begin(), collision_tile_layers_.end(), &layer);
    collision_tile_layers_.erase(it, collision_tile_layers_.end());
    mylog::MyLog{}.trace("unregister collision layer success");
}

void PhysicsEngine::resolveTileCollisions(engine::component::PhysicsComponent& pc, float dt) {
    // 检查组件是否有效
    auto* obj = pc.getOwner();
    if (!obj) return;
    auto* tc = obj->getComponent<engine::component::TransformComponent>();
    auto* cc = obj->getComponent<engine::component::ColliderComponent>();
    if (!tc || !cc || !cc->isActive() || cc->isTrigger()) return;
    auto world_aabb = cc->getWorldAABB(); // 使用最小包围盒进行碰撞检测（简化）
    auto obj_pos = world_aabb.position;
    auto obj_size = world_aabb.size;
    if (world_aabb.size.x <= 0.0f || world_aabb.size.y <= 0.0f) return;
    // -- 检查结束, 正式开始处理 --

    auto tolerance = 1.0f;           // 检查右边缘和下边缘时，需要减1像素，否则会检查到下一行/列的瓦片
    auto ds = pc.getVelocity() * dt; // 计算物体在delta_time内的位移
    auto new_obj_pos = obj_pos + ds; // 计算物体在delta_time后的新位置

    // 遍历所有注册的碰撞瓦片层
    for (auto* layer: collision_tile_layers_) {
        if (!layer) continue;
        auto tile_size = layer->getTileSize();
        // 轴分离碰撞检测：先检查X方向是否有碰撞 (y方向使用初始值obj_pos.y)
        if (ds.x > 0.0f) {
            // 检查右侧碰撞，需要分别测试右上和右下角
            auto right_top_x = new_obj_pos.x + obj_size.x;
            auto tile_x = static_cast<int>(floor(right_top_x / tile_size.x)); // 获取x方向瓦片坐标
            // y方向坐标有两个，右上和右下
            auto tile_y = static_cast<int>(floor(obj_pos.y / tile_size.y));
            auto tile_type_top = layer->getTileTypeAt({tile_x, tile_y}); // 右上角瓦片类型
            auto tile_y_bottom = static_cast<int>(floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));
            auto tile_type_bottom = layer->getTileTypeAt({tile_x, tile_y_bottom}); // 右下角瓦片类型

            if (tile_type_top == engine::component::TileType::SOLID ||
                tile_type_bottom == engine::component::TileType::SOLID) {
                // 撞墙了！速度归零，x方向移动到贴着墙的位置
                new_obj_pos.x = tile_x * layer->getTileSize().x - obj_size.x;
                pc.setVelocityX(0.0f);
                pc.setCollidedRight(true);
            } else {
                // 处理右下斜坡的瓦片
                auto width_right = new_obj_pos.x + obj_size.x - tile_x * tile_size.x;
                auto height_right = getTileHeightWidth(width_right, tile_type_bottom, tile_size);
                if (height_right > 0.0f) {
                    float new_y = (tile_y_bottom + 1.0f) * layer->getTileSize().y - obj_size.y - height_right;
                    if (new_obj_pos.y > new_y) { new_obj_pos.y = new_y; }
                    pc.setCollidedBelow(true);
                }
            }
        } else if (ds.x < 0.0f) {
            // 检查左侧碰撞，需要分别测试左上和左下角
            auto left_top_x = new_obj_pos.x;
            auto tile_x = static_cast<int>(floor(left_top_x / tile_size.x)); // 获取x方向瓦片坐标
            // y方向坐标有两个，左上和左下
            auto tile_y = static_cast<int>(floor(obj_pos.y / tile_size.y));
            auto tile_type_top = layer->getTileTypeAt({tile_x, tile_y}); // 左上角瓦片类型
            auto tile_y_bottom = static_cast<int>(floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));
            auto tile_type_bottom = layer->getTileTypeAt({tile_x, tile_y_bottom}); // 左下角瓦片类型

            if (tile_type_top == engine::component::TileType::SOLID || tile_type_bottom ==
                engine::component::TileType::SOLID) {
                // 撞墙了！速度归零，x方向移动到贴着墙的位置
                new_obj_pos.x = static_cast<float>((tile_x + 1) * layer->getTileSize().x);
                pc.setVelocityX(0.0f);
                pc.setCollidedLeft(true);
            } else {
                auto width_left = new_obj_pos.x - tile_x * tile_size.x;
                auto height_left = getTileHeightWidth(width_left, tile_type_bottom, tile_size);
                if (height_left > 0.0f) {
                    float new_y = (tile_y_bottom + 1.0f) * layer->getTileSize().y - obj_size.y - height_left;
                    if (new_obj_pos.y > new_y) { new_obj_pos.y = new_y; }

                    pc.setCollidedBelow(true);
                }
            }
        }
        // 轴分离碰撞检测：再检查Y方向是否有碰撞 (x方向使用初始值obj_pos.x)
        if (ds.y > 0.0f) {
            // 检查底部碰撞，需要分别测试左下和右下角
            auto bottom_left_y = new_obj_pos.y + obj_size.y;
            auto tile_y = static_cast<int>(floor(bottom_left_y / tile_size.y));

            auto tile_x = static_cast<int>(floor(obj_pos.x / tile_size.x));
            auto tile_type_left = layer->getTileTypeAt({tile_x, tile_y}); // 左下角瓦片类型
            auto tile_x_right = static_cast<int>(floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));
            auto tile_type_right = layer->getTileTypeAt({tile_x_right, tile_y}); // 右下角瓦片类型

            if (tile_type_left == engine::component::TileType::SOLID ||
                tile_type_right == engine::component::TileType::SOLID ||
                tile_type_left == engine::component::TileType::UNISOLID ||
                tile_type_right == engine::component::TileType::UNISOLID) {
                // 到达地面！速度归零，y方向移动到贴着地面的位置
                new_obj_pos.y = tile_y * layer->getTileSize().y - obj_size.y;
                pc.setVelocityY(0.0f);
                pc.setCollidedBelow(true);
            } else {
                // 检测斜坡瓦片 下方两个点都要检测；
                auto width_left = obj_pos.x - tile_x * tile_size.x;
                auto width_right = obj_pos.x + obj_size.x - tile_x_right * tile_size.x;
                auto height_left = getTileHeightWidth(width_left, tile_type_left, tile_size);
                auto height_right = getTileHeightWidth(width_right, tile_type_right, tile_size);
                auto height = glm::max(height_left, height_right); // 找到两个角点最高的进行检测；
                if (height > 0.0f) {
                    // 至少一个焦点处于斜坡瓦片；
                    float new_y = (tile_y + 1) * layer->getTileSize().y - obj_size.y - height;
                    if (new_obj_pos.y > new_y) { new_obj_pos.y = new_y; }
                    pc.setVelocityY(0.0f);
                    pc.setCollidedBelow(true);
                }
            }
        } else if (ds.y < 0.0f) {
            // 检查顶部碰撞，需要分别测试左上和右上角
            auto top_left_y = new_obj_pos.y;
            auto tile_y = static_cast<int>(floor(top_left_y / tile_size.y));

            auto tile_x = static_cast<int>(floor(obj_pos.x / tile_size.x));
            auto tile_type_left = layer->getTileTypeAt({tile_x, tile_y}); // 左上角瓦片类型
            auto tile_x_right = static_cast<int>(floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));
            auto tile_type_right = layer->getTileTypeAt({tile_x_right, tile_y}); // 右上角瓦片类型

            if (tile_type_left == engine::component::TileType::SOLID || tile_type_right ==
                engine::component::TileType::SOLID) {
                // 撞到天花板！速度归零，y方向移动到贴着天花板的位置
                new_obj_pos.y = static_cast<float>(tile_y + 1) * layer->getTileSize().y;
                pc.setVelocityY(0.0f);
                pc.setCollidedAbove(true);
            }
        }
    }
    // 更新物体位置，并限制最大速度
    tc->translate(new_obj_pos - obj_pos);
}

void PhysicsEngine::resolveSolidObjectCollision(engine::object::GameObject& mov_obj,
                                                engine::object::GameObject& solid_obj) {
    auto move_tc = mov_obj.getComponent<engine::component::TransformComponent>();
    auto move_pc = mov_obj.getComponent<engine::component::PhysicsComponent>();
    auto move_cc = mov_obj.getComponent<engine::component::ColliderComponent>();
    auto solid_cc = solid_obj.getComponent<engine::component::ColliderComponent>();

    auto move_aabb = move_cc->getWorldAABB();
    auto solid_aabb = solid_cc->getWorldAABB();

    auto move_center = move_aabb.position + move_aabb.size / 2.0f;
    auto solid_center = solid_aabb.position + solid_aabb.size / 2.0f;

    // 计算两个包围盒重叠的部分；
    auto overlap = glm::vec2{move_aabb.size / 2.0f + solid_aabb.size / 2.0f} - glm::abs(move_center - solid_center);
    // 重叠范围过小；
    if (overlap.x < 0.1f && overlap.y < 0.1f) { return; }
    if (overlap.x < overlap.y) {
        // 如果重叠部分在x方向上更小，则认为碰撞发生在x方向上（推出x方向平移向量最小）
        if (move_center.x < solid_center.x) {
            // 移动物体在左侧；让它贴着右边SOLID物体（相当于向左移出重叠部分），y方向正常移动
            move_tc->translate(glm::vec2{-overlap.x, 0.0f});
            // 速度为正  （向右） 则归零；
            if (move_pc->getVelocity().x > 0.0f) {
                move_pc->setVelocityX(0.0f);
                move_pc->setCollidedRight(true);
            }
        } else {
            move_tc->translate(glm::vec2{overlap.x, 0.0f});
            if (move_pc->getVelocity().x < 0.0f) {
                move_pc->setVelocityX(0.0f);
                move_pc->setCollidedLeft(true);
            }
        }
    } else {
        if (move_center.y < solid_center.y) {
            // 移动物体在上侧；
            move_tc->translate(glm::vec2{0.0f, -overlap.y});
            if (move_pc->getVelocity().y < 0.0f) {
                move_pc->setVelocityY(0.0f);
                move_pc->setCollidedBelow(true);
            }
        } else {
            move_tc->translate(glm::vec2{0.0f, overlap.y});
            if (move_pc->getVelocity().y < 0.0f) {
                move_pc->setVelocityY(0.0f);
                move_pc->setCollidedAbove(true);
            }
        }
    }
}

void PhysicsEngine::applyWorldBounds(engine::component::PhysicsComponent& pc) {
    if (!world_bounds_) return;

    // 获取数据；
    auto* obj = pc.getOwner();
    auto* cc = obj->getComponent<engine::component::ColliderComponent>();
    if (!cc) return;
    auto* tc = obj->getComponent<engine::component::TransformComponent>();
    auto world_aabb = cc->getWorldAABB();
    auto obj_pos = world_aabb.position;
    auto obj_size = world_aabb.size;

    // 只限定左上右边界；  不限制下边界；  以碰撞盒作为判断依据
    // 检查左边界；
    if (obj_pos.x < world_bounds_->position.x) {
        pc.setVelocityX(0.0f);
        obj_pos.x = world_bounds_->position.x;
        pc.setCollidedLeft(true);
    }

    // 检查上边界
    if (obj_pos.y < world_bounds_->position.y) {
        pc.setVelocityY(0.0f);
        obj_pos.y = world_bounds_->position.y;
        pc.setCollidedAbove(true);
    }

    // 检查右边界；
    if (obj_pos.x > world_bounds_->position.x + world_bounds_->size.x) {
        pc.setVelocityX(0.0f);
        obj_pos.x = world_bounds_->position.x + world_bounds_->size.x;
        pc.setCollidedRight(true);
    }

    // 更新物体位置
    tc->translate(obj_pos - world_aabb.position);
}

float PhysicsEngine::getTileHeightWidth(float width, engine::component::TileType type, glm::vec2 tile_size) {
    auto rel_x = glm::clamp(width / tile_size.x, 0.0f, 1.0f);
    switch (type) {
        case engine::component::TileType::SLOPE_0_1: // 左0  右1
            return rel_x * tile_size.y;
        case engine::component::TileType::SLOPE_0_2: // 左0  右1/2
            return rel_x * tile_size.y * 0.5f;
        case engine::component::TileType::SLOPE_2_1: // 左1/2    右1；
            return rel_x * tile_size.y * 0.5f + tile_size.y * 0.5f;
        case engine::component::TileType::SLOPE_1_0: // 左1  右0
            return (1.0f - rel_x) * tile_size.y;
        case engine::component::TileType::SLOPE_2_0: // 左1/2    右0
            return (1.0f - rel_x) * tile_size.y * 0.5f;
        case engine::component::TileType::SLOPE_1_2: // 左1  右1/2；
            return (1.0f - rel_x) * tile_size.y * 0.5f + tile_size.y * 0.5f;
        default:
            return 0.0f;
    }
}

void PhysicsEngine::update(const std::chrono::milliseconds& dt) {
    // 清空前一帧的碰撞对；
    collision_pairs_.clear();
    // 遍历所有注册的物理组件
    for (const auto& component: components_) {
        if (!component->isEnabled()) { continue; }

        component->resetCollisionFlags();

        float dt_s = std::chrono::duration<float>(dt).count();
        // 应用重力；  v -= g * t;
        if (component->isUseGravity()) {
            glm::vec2 g{gravity_ * dt_s};
            component->addVelocity(g);
        }
        // 更新速度； v += a * dt; a = F / m;
        auto velocity = (component->getForce() / component->getMass()) * dt_s;
        component->addVelocity(velocity);
        component->clearForce();

        resolveTileCollisions(*component, dt_s);

        applyWorldBounds(*component);
    }

    // 检查碰撞；
    checkObjectCollisions();
}

void PhysicsEngine::checkObjectCollisions() {
    for (int i = 0; i < components_.size() - 1; ++i) {
        auto& pc_a = components_[i];
        if (!pc_a->isEnabled()) { continue; }
        auto* obj_a = pc_a->getOwner();
        if (!obj_a) { continue; }
        auto* cc_a = obj_a->getComponent<engine::component::ColliderComponent>();
        if (!cc_a || !cc_a->isActive()) { continue; }

        for (int j = i + 1; j < components_.size(); ++j) {
            auto& pc_b = components_[j];
            if (!pc_b->isEnabled()) { continue; }
            auto* obj_b = pc_b->getOwner();
            if (!obj_b) { continue; }
            auto* cc_b = obj_b->getComponent<engine::component::ColliderComponent>();
            if (!cc_b || !cc_b->isActive()) { continue; }

            if (engine::physics::collision::checkCollision(*cc_a, *cc_b)) {
                // 可移动物体与solid物体 碰撞； 直接处理位置变化；
                if (obj_a->getTag() != SOLID && obj_b->getTag() == SOLID) {
                    resolveSolidObjectCollision(*obj_a, *obj_b);
                } else if (obj_a->getTag() == SOLID && obj_b->getTag() != SOLID) {
                    resolveSolidObjectCollision(*obj_b, *obj_a);
                } else {
                    // 记录碰撞
                    collision_pairs_.emplace_back(obj_a, obj_b);
                }
            }
        }
    }
}
} // physics
} // engine
