//
// Created by wangm on 26-10-19.
//

#include "level_loader.h"

#include <fstream>
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>

#include "scene.h"
#include "../component/parallax_component.h"
#include "../component/transform_component.h"
#include "../log/my_log.h"
#include "../object/game_object.h"
#include "../component/tilelayer_component.h"
#include "../object/game_object.h"
#include <memory>

#include "../component/animation_component.h"
#include "../component/collider_component.h"
#include "../component/physics_component.h"
#include "../component/sprite_component.h"
#include "../core/context.h"
#include "../physics/aabb_collider.h"
#include "../render/animation.h"

namespace engine {
namespace scene {
constexpr static std::string_view OFFSET_X{"offsetx"};
constexpr static std::string_view OFFSET_Y{"offsety"};
constexpr static std::string_view PARALLAX_X{"parallaxx"};
constexpr static std::string_view PARALLAX_Y{"parallaxy"};
constexpr static std::string_view REPEAT_X{"repeatx"};
constexpr static std::string_view REPEAT_Y{"repeaty"};
constexpr static std::string_view NAME{"name"};
static std::string NONE{NONE};
constexpr static std::string_view WIDTH{"width"};
constexpr static std::string_view HEIGHT{"height"};
constexpr static std::string_view TILE_WIDTH{"tileheight"};
constexpr static std::string_view TILE_HEIGHT{"tileheight"};
constexpr static std::string_view TILESETS{"tilesets"};
constexpr static std::string_view SOURCE{"source"};
constexpr static std::string_view FIRST_GID{"firstgid"};
constexpr static std::string_view FILE_PATH{"file_path"};
constexpr static std::string_view IMAGE{"image"};
constexpr static std::string_view IMAGE_WIDTH{"imagewidth"};
constexpr static std::string_view IMAGE_HEIGHT{"imageheight"};
constexpr static std::string_view COLUMNS{"columns"};
constexpr static std::string_view TILES{"tiles"};
constexpr static std::string_view ID{"id"};
constexpr static std::string_view DATA{"data"};
constexpr static std::string_view OBJECTS{"objects"};
constexpr static std::string_view GID{"gid"};
constexpr static std::string_view X{"x"};
constexpr static std::string_view Y{"y"};
constexpr static std::string_view ROTATION{"rotation"};
constexpr static std::string_view PROPERTIES{"properties"};
constexpr static std::string_view VALUE{"value"};
constexpr static std::string_view SOLID{"solid"};
constexpr static std::string_view OBJECT_GROUP{"objectgroup"};
constexpr static std::string_view TAG{"tag"};
constexpr static std::string_view GRAVITY{"gravity"};
static std::string SLOPE{"slope"};
constexpr static std::string_view UNISOLID{"unisolid"};
constexpr static std::string_view DURATION{"duration"};
constexpr static std::string_view ROW{"row"};
constexpr static std::string_view FRAMES{"frames"};
constexpr static std::string_view ANIMATION{"animation"};


bool LevelLoader::loadLevel(const std::string& map_path, Scene& scene) {
    // 加载图层文件
    std::ifstream file{map_path};
    if (!file.is_open()) {
        mylog::MyLog{}.error("loadLevel;cannot open file; map_path = {}; scene = {}", map_path, scene.getSceneName());
        return false;
    }

    // 解析数据
    nlohmann::json json_data;
    try {
        file >> json_data;
    } catch (const std::exception& e) {
        mylog::MyLog{}.error("loadLevel,file to json failed;map_path = {},scene = {}，error = {}",
                             map_path, scene.getSceneName(), e.what());
        return false;
    }

    // 加载地图信息
    map_path_ = map_path;
    map_size_ = glm::ivec2(json_data.value(WIDTH, 0), json_data.value(HEIGHT, 0));
    tile_size_ = glm::ivec2(json_data.value(TILE_WIDTH, 0), json_data.value(TILE_HEIGHT, 0));

    // 加载瓦片数据
    if (json_data.contains(TILESETS) && json_data[TILESETS].is_array()) {
        for (const auto& tileset: json_data[TILESETS]) {
            if (!tileset.contains(SOURCE) || !tileset[SOURCE].is_string() ||
                !tileset.contains(FIRST_GID) || !tileset[FIRST_GID].is_number_integer()) {
                mylog::MyLog{}.error("tileset not have 'source' or 'firstgid';map_path = {},scene = {}",
                                     map_path, scene.getSceneName());
                continue;
            }
            auto tileset_path = resolvePath(tileset[SOURCE], map_path);
            auto first_gid = tileset[FIRST_GID];
            loadTileset(tileset_path, first_gid);
        }
    }

    // 加载图层数据
    if (!json_data.contains("layers") || !json_data["layers"].is_array()) {
        mylog::MyLog{}.error("map_file '{}' layers is not exist/valid ", map_path);
        return false;
    }

    for (const auto& layer: json_data["layers"]) {
        if (!layer.value("visible", true)) {
            spdlog::info("layer invisible;continue");
            continue;
        }
        std::string layer_type = layer.value("type", NONE);
        if (layer_type == "imagelayer") {
            loadImageLayer(layer, scene);
        } else if (layer_type == "tilelayer") {
            loadTileLayer(layer, scene);
        } else if (layer_type == "objectgroup") {
            loadObjectLayer(layer, scene);
        } else {
            mylog::MyLog{}.warn("layer type is not support;layer_type = {}", layer_type);
        }
    }
    mylog::MyLog{}.info("loadLevel success;map_path = {},scene = {}", map_path, scene.getSceneName());
    return true;
}

void LevelLoader::loadImageLayer(const nlohmann::json& layer_json, Scene& scene) {
    const std::string& image_path = layer_json.value("image", "");
    if (image_path.empty()) {
        mylog::MyLog{}.error("layer '{}' need 'image'", layer_json.value(NAME, NONE));
        return;
    }

    auto texture_id = resolvePath(image_path, map_path_);
    if (texture_id.empty()) {
        mylog::MyLog{}.error("layer '{}' image_path is invalid;image_path = {}", layer_json.value(NAME, NONE),
                             image_path);
        return;
    }

    // 获取图层偏移量
    const glm::vec2 offset = glm::vec2(layer_json.value(OFFSET_X, 0.0f), layer_json.value(OFFSET_Y, 0.0f));
    // 获取视差因子及重复标志；
    const glm::vec2 scroll_factor = glm::vec2{layer_json.value(PARALLAX_X, 0.0f), layer_json.value(PARALLAX_Y, 0.0f)};
    const glm::bvec2 repeat = glm::bvec2{layer_json.value(REPEAT_X, false), layer_json.value(REPEAT_Y, false)};

    // 获取图层名
    const std::string& layer_name = layer_json.value(NAME, NONE);

    // 创建游戏对象
    auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
    game_object->addComponent<engine::component::TransformComponent>(offset);
    game_object->addComponent<engine::component::ParallaxComponent>(texture_id, scroll_factor, repeat);
    scene.addGameObject(std::move(game_object));
    mylog::MyLog{}.info("load layer '{}' success;", layer_name);
}

void LevelLoader::loadTileLayer(const nlohmann::json& layer_json, Scene& scene) {
    if (!layer_json.contains(DATA) || !layer_json[DATA].is_array()) {
        mylog::MyLog{}.error("layer '{}' need 'data'", layer_json.value(NAME, NONE));
        return;
    }
    std::vector<engine::component::TileInfo> tiles;
    tiles.reserve(map_size_.x * map_size_.y);
    const auto& data = layer_json[DATA];
    for (const auto& gid: data) {
        tiles.push_back(getTileInfoByGid(gid));
    }
    const std::string& layer_name = layer_json.value(NAME, NONE);
    auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
    game_object->addComponent<engine::component::TileLayerComponent>(tile_size_, map_size_, std::move(tiles));
    scene.addGameObject(std::move(game_object));
}

void LevelLoader::loadObjectLayer(const nlohmann::json& layer_json, Scene& scene) {
    if (!layer_json.contains(OBJECTS) || !layer_json[OBJECTS].is_array()) {
        mylog::MyLog{}.error("layer '{}' need 'object'", layer_json.value(NAME, NONE));
        return;
    }
    const auto& objects = layer_json[OBJECTS];
    for (const auto& object: objects) {
        auto gid = object.value(GID, 0);
        if (gid == 0) {
            // gid 不存在 代表需要自行处理；
            // TODO;
            continue;
        }
        // 根据gid获取信息；
        auto tile_info = getTileInfoByGid(gid);
        if (tile_info.sprite.getTextureId().empty()) {
            mylog::MyLog{}.error("gid = '{}' need texture_id", gid);
            continue;
        }
        // 获取transform信息；
        auto position = glm::vec2(object.value(X, 0.0f), object.value(Y, 0.0f));
        auto dst_size = glm::vec2(object.value(WIDTH, 0.0f), object.value(HEIGHT, 0.0f));
        position = glm::vec2(position.x, position.y - dst_size.y); // 对齐到左下角；

        auto rotation = object.value(ROTATION, 0.0f);
        auto src_size_opt = tile_info.sprite.getSourceRect();
        if (!src_size_opt) {
            mylog::MyLog{}.error("tile need sourceRect; gid = '{}'", gid);
            continue;
        }
        auto src_size = glm::vec2(src_size_opt->w, src_size_opt->h);
        auto scale = dst_size / src_size;

        // 获取对象名称
        const std::string& object_name = object.value(NAME, NONE);

        // 创建游戏对象并添加组件；
        std::unique_ptr<engine::object::GameObject> game_object = std::make_unique<
            engine::object::GameObject>(object_name);
        game_object->addComponent<engine::component::TransformComponent>(position, scale, rotation);
        game_object->addComponent<engine::component::SpriteComponent>(std::move(tile_info.sprite),
                                                                      scene.getContext().getResourceManager());

        // 获取瓦片json信息；
        auto tile_json = getTileJsonByGid(gid);

        // 获取碰撞信息 如果是solid添加物理组件；
        if (tile_info.type == engine::component::TileType::SOLID) {
            auto collider = std::make_unique<engine::physics::AABBCollider>(src_size);
            // 添加物理组件  组件不受重力影响；
            game_object->addComponent<engine::component::PhysicsComponent>(scene.getContext().getPhysicsEngine(),
                                                                           false);
            game_object->setTag(std::string(SOLID)); // 设置瓦片标签
        } else if (auto rect = getColliderRect(tile_json.value()); rect) {
            auto collider = std::make_unique<engine::physics::AABBCollider>(rect->size);
            // 添加物理组件  组件不受重力影响；
            engine::component::ColliderComponent* cc = game_object->addComponent<
                engine::component::ColliderComponent>(std::move(collider));
            cc->setOffset(rect->position);
            game_object->addComponent<engine::component::PhysicsComponent>(scene.getContext().getPhysicsEngine(),
                                                                           false);
        }
        auto tag = getTileProperty<std::string>(tile_json.value(), std::string{TAG});
        if (tag.has_value()) { game_object->setTag(tag.value()); }

        // 获取并设置重力信息
        auto gravity = getTileProperty<bool>(tile_json.value(), std::string{GRAVITY});
        if (gravity) {
            engine::component::PhysicsComponent* pc = game_object->getComponent<engine::component::PhysicsComponent>();
            if (pc) {
                pc->setUseGravity(gravity.value());
            } else {
                game_object->addComponent<engine::component::PhysicsComponent>(scene.getContext().getPhysicsEngine(),
                                                                               gravity.value());
            }
        }

        // 获取并设置动画
        auto anim_string = getTileProperty<std::string>(tile_json.value(), std::string{ANIMATION});
        if (anim_string) {
            nlohmann::json anim_json;
            try {
                anim_json = nlohmann::json::parse(anim_string.value());
            } catch (const nlohmann::json::parse_error& e) {
                mylog::MyLog{}.error("parse anim_string error: {}", e.what());
                continue;
            }
            auto* ac = game_object->addComponent<engine::component::AnimationComponent>();
            addAnimation(anim_json, *ac, src_size);
        }

        scene.addGameObject(std::move(game_object)); // 设置标签
        mylog::MyLog{}.info("load Object {} success", object_name);
    }
}

void LevelLoader::addAnimation(const nlohmann::json& anim_json, engine::component::AnimationComponent& ac,
                               const glm::vec2& sprite_size) {
    if (!anim_json.is_object()) {
        mylog::MyLog{}.error("anim_json is not object");
        return;
    }
    for (const auto& anim: anim_json.items()) {
        const std::string& anim_name = anim.key();
        const auto& anim_info = anim.value();
        if (!anim_info.is_object()) {
            mylog::MyLog{}.warn("anim_info is not object");
            continue;
        }

        auto duration_ms = anim_info.value(DURATION, 100);         // 获取持续时间； 默认100ms
        auto duration = static_cast<float>(duration_ms) / 1000.0f; // 转换成秒；
        auto row = anim_info.value(ROW, 0);                        // 获取行数；

        if (!anim_info.contains(FRAMES) || !anim_info[FRAMES].is_array()) {
            mylog::MyLog{}.warn("anim_info '{}' is not array", anim_name);
            continue;
        }

        auto animation = std::make_unique<engine::render::Animation>(anim_name);
        for (const auto& frame: anim_info[FRAMES]) {
            if (!frame.is_number_integer()) {
                mylog::MyLog{}.warn("frame is not number_integer");
                continue;
            }
            auto column = frame.get<int>();
            // 计算源矩形；
            SDL_FRect src_rect = {
                column * sprite_size.x,
                row * sprite_size.y,
                sprite_size.x,
                sprite_size.y
            };
            // 添加动画帧
            animation->addFrame(src_rect, duration);
        }
        ac.addAnimation(std::move(animation));
    }
}

std::optional<engine::utils::Rect> LevelLoader::getColliderRect(const nlohmann::json& tile_json) {
    if (!tile_json.contains(OBJECT_GROUP)) { return std::nullopt; }
    auto& object_group = tile_json[OBJECT_GROUP];
    if (!object_group.contains(OBJECTS)) { return std::nullopt; }
    auto& objects = object_group[OBJECTS];
    for (const auto& object: objects) {
        auto rect = engine::utils::Rect{
            glm::vec2{object.value(X, 0.0f), object.value(Y, 0.0f)},
            glm::vec2{object.value(WIDTH, 0.0f), object.value(HEIGHT, 0.0f)}
        };
        if (rect.size.x > 0 && rect.size.y > 0) {
            return rect;
        }
    }
    return std::nullopt;
}

std::optional<nlohmann::json> LevelLoader::getTileJsonByGid(int gid) const {
    // 查找tileset中  键小于等于 gid 的最近的元素
    auto tileset_it = tileset_data_.upper_bound(gid);
    if (tileset_it == tileset_data_.begin()) { return std::nullopt; }
    --tileset_it;
    // 获取图块集json 对象
    const auto& tileset = tileset_it->second;
    auto local_id = gid - tileset_it->first;
    if (!tileset.contains(TILES)) { return std::nullopt; }
    const auto& tiles_json = tileset[TILES];
    for (const auto& tile_json: tiles_json) {
        auto tile_id = tile_json.value(ID, 0);
        if (tile_id == local_id) {
            return tile_json;
        }
    }
    return std::nullopt;
}


engine::component::TileInfo LevelLoader::getTileInfoByGid(int gid) {
    if (gid == 0) { return engine::component::TileInfo(); }
    auto tileset_it = tileset_data_.upper_bound(gid); // 找到大于gid 的第一个元素
    if (tileset_it == tileset_data_.begin()) {
        mylog::MyLog{}.error("tileset_data_ is empty;gid = {}", gid);
        return engine::component::TileInfo();
    }
    --tileset_it; // 前移一位，  这样就得到不大于gid的最后一个元素
    const auto& tileset = tileset_it->second;
    auto local_id = gid - tileset_it->first; // 计算局部id；
    const std::string file_path = tileset.value(FILE_PATH, "");
    if (file_path.empty()) {
        mylog::MyLog{}.error("tileset '{}' have no file_path;gid = {}", tileset.value(NAME, NONE), gid);
        return engine::component::TileInfo();
    }
    // 图集分两种情况
    if (tileset.contains(IMAGE)) {
        // 单图片情况
        auto texture_id = resolvePath(tileset[IMAGE].get<std::string>(), file_path);
        auto coordinate_x = local_id % tileset[COLUMNS].get<int>();
        auto coordinate_y = local_id / tileset[COLUMNS].get<int>();
        SDL_FRect texture_rect{
            static_cast<float>(coordinate_x) * tile_size_.x,
            static_cast<float>(coordinate_y) * tile_size_.y,
            static_cast<float>(tile_size_.x),
            static_cast<float>(tile_size_.y)
        };
        engine::render::Sprite sprite{texture_id, texture_rect};
        return engine::component::TileInfo{sprite, getTileTypeById(tileset, local_id)};
    } else {
        // 多图片情况
        if (!tileset.contains(TILES)) {
            mylog::MyLog{}.error("tileset '{}' have no tiles;gid = {}", tileset_it->first, gid);
            return engine::component::TileInfo();
        }
        const auto& tiles_json = tileset[TILES];
        for (const auto& tile_json: tiles_json) {
            auto tile_id = tile_json.value(ID, 0);
            if (!(tile_id == local_id)) { continue; }
            if (!tile_json.contains(IMAGE)) {
                mylog::MyLog{}.error("tile '{}' have no image;gid = {}", tileset_it->first, gid);
                return engine::component::TileInfo();
            }
            auto texture_id = resolvePath(tile_json[IMAGE].get<std::string>(), file_path);
            auto image_width = tile_json.value(IMAGE_WIDTH, 0);
            auto image_height = tile_json.value(IMAGE_HEIGHT, 0);

            SDL_FRect texture_rect{
                static_cast<float>(tile_json.value("x", 0)),
                static_cast<float>(tile_json.value("y", 0)),
                static_cast<float>(tile_json.value(WIDTH, image_width)),
                static_cast<float>(tile_json.value(HEIGHT, image_height))
            };
            engine::render::Sprite sprite{texture_id, texture_rect};
            return engine::component::TileInfo{sprite, getTileType(tile_json)};
        }
    }
    mylog::MyLog{}.error("tileset '{}' have no tile;gid = {}", tileset_it->first, gid);
    return engine::component::TileInfo();
}

void LevelLoader::loadTileset(const std::string& tileset_path, int first_gid) {
    std::ifstream tileset_file(tileset_path);
    if (!tileset_file.is_open()) {
        mylog::MyLog{}.error("tileset_file open failed;tileset_path = {}", tileset_path);
        return;
    }
    nlohmann::json json;
    try {
        tileset_file >> json;
    } catch (const std::exception& e) {
        mylog::MyLog{}.error("tileset_file read failed;tileset_path = {};error = {}", tileset_path, e.what());
        return;
    }
    json[FILE_PATH] = tileset_path;
    tileset_data_[first_gid] = std::move(json);
    spdlog::info("load tileset success;tileset_path = {}", tileset_path);
}

std::string LevelLoader::resolvePath(const std::string& relative_path, const std::string& file_path) {
    try {
        auto map_dir = std::filesystem::path(file_path).parent_path();
        auto final_path = std::filesystem::canonical(map_dir / relative_path);
        return final_path.string();
    } catch (const std::exception& e) {
        spdlog::error("resolvePath failed;image_path = {};error = {}", relative_path, e.what());
        return "";
    }
}

engine::component::TileType LevelLoader::getTileType(const nlohmann::json& tile_json) {
    if (!tile_json.contains(PROPERTIES)) { return engine::component::TileType::NORMAL; }
    auto& properties = tile_json[PROPERTIES];
    for (auto& property: properties) {
        if (!property.contains(NAME)) { continue; }
        const auto& property_name = property[NAME];
        if (property_name == "solid") {
            auto is_solid = property.value(VALUE, false);
            return is_solid ? engine::component::TileType::SOLID : engine::component::TileType::NORMAL;
        } else if (property_name == SLOPE) {
            auto slope_type = property.value(VALUE, "");
            if (slope_type == "0_1")return engine::component::TileType::SLOPE_0_1;
            else if (slope_type == "1_0")return engine::component::TileType::SLOPE_1_0;
            else if (slope_type == "0_2")return engine::component::TileType::SLOPE_0_2;
            else if (slope_type == "2_0") return engine::component::TileType::SLOPE_2_0;
            else if (slope_type == "1_2") return engine::component::TileType::SLOPE_1_2;
            else if (slope_type == "2_1") return engine::component::TileType::SLOPE_2_1;
            else {
                spdlog::error("unknown slope type;type = {}", slope_type);
                return engine::component::TileType::NORMAL;
            }
        } else if (property_name == UNISOLID.data()) {
            auto is_unisolid = property.value(VALUE, false);
            return is_unisolid ? engine::component::TileType::UNISOLID : engine::component::TileType::NORMAL;
        }
    }
    return engine::component::TileType::NORMAL;
}

engine::component::TileType LevelLoader::getTileTypeById(const nlohmann::json& tileset_json, int local_id) {
    if (!tileset_json.contains(TILES)) { return engine::component::TileType::NORMAL; }
    auto& tiles = tileset_json[TILES];
    for (auto& tile: tiles) {
        if (tile.contains(ID) && tile[ID] == local_id) {
            return getTileType(tile);
        }
    }
    return engine::component::TileType::NORMAL;
}
} // scene
} // engine
