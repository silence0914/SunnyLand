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

#include "../component/sprite_component.h"
#include "../core/context.h"

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
        mylog::MyLog{}.error("loadLevel,file to json failed;map_path = {},scene = {}", map_path, scene.getSceneName());
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
            mylog::MyLog{}.error("gid = '{}' need texture_id",gid);
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
        auto game_object = std::make_unique<engine::object::GameObject>(object_name);
        game_object->addComponent<engine::component::TransformComponent>(position, scale, rotation);
        game_object->addComponent<engine::component::SpriteComponent>(std::move(tile_info.sprite),
                                                                      scene.getContext().getResourceManager());
        scene.addGameObject(std::move(game_object));
        mylog::MyLog{}.info("load Object {} success", object_name);
    }
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
        return engine::component::TileInfo{sprite, engine::component::TileType::NORMAL};
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
            return engine::component::TileInfo{sprite, engine::component::TileType::NORMAL};
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
} // scene
} // engine
