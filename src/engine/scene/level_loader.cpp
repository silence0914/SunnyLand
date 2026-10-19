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

namespace engine {
namespace scene {
constexpr static std::string_view offset_x{"offsetx"};
constexpr static std::string_view offset_y{"offsety"};
constexpr static std::string_view parallax_x{"parallaxx"};
constexpr static std::string_view parallax_y{"parallaxy"};
constexpr static std::string_view repeat_x{"repeatx"};
constexpr static std::string_view repeat_y{"repeaty"};
constexpr static std::string_view name{"name"};
constexpr static std::string_view none{"none"};


bool LevelLoader::loadLevel(const std::string& map_path, Scene& scene) {
    // 加载图层文件
    std::ifstream file{map_path};
    if (!file.is_open()) {
        mylog::MyLog{}.error("loadLevel;cannot open file; map_path = {}; scene = {}", map_path, scene.getSceneName());
        return false;
    }
    map_path_ = map_path;

    // 解析数据
    nlohmann::json json_data;
    try {
        file >> json_data;
    } catch (const std::exception& e) {
        mylog::MyLog{}.error("loadLevel,file to json failed;map_path = {},scene = {}", map_path, scene.getSceneName());
        return false;
    }

    // 加载数据
    if (!json_data.contains("layers") || !json_data["layers"].is_array()) {
        mylog::MyLog{}.error("map_file '{}' layers is not exist/valid ", map_path);
        return false;
    }

    for (const auto& layer: json_data["layers"]) {
        if (!layer.value("visible", true)) {
            spdlog::info("layer invisible;continue");
            continue;
        }
        std::string layer_type = layer.value("type", "none");
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
        mylog::MyLog{}.error("layer '{}' need 'image'", layer_json.value(name, "none"));
        return;
    }

    auto texture_id = resolvePath(image_path);
    if (texture_id.empty()) {
        mylog::MyLog{}.error("layer '{}' image_path is invalid;image_path = {}", layer_json.value(name, "none"),
                             image_path);
        return;
    }

    // 获取图层偏移量
    const glm::vec2 offset = glm::vec2(layer_json.value(offset_x, 0.0f), layer_json.value(offset_y, 0.0f));
    // 获取视差因子及重复标志；
    const glm::vec2 scroll_factor = glm::vec2{layer_json.value(parallax_x, 0.0f), layer_json.value(parallax_y, 0.0f)};
    const glm::bvec2 repeat = glm::bvec2{layer_json.value(repeat_x, false), layer_json.value(repeat_y, false)};

    // 获取图层名
    const std::string& layer_name = layer_json.value(name, "none");

    // 创建游戏对象
    auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
    game_object->addComponent<engine::component::TransformComponent>(offset);
    game_object->addComponent<engine::component::ParallaxComponent>(texture_id, scroll_factor, repeat);
    scene.addGameObject(std::move(game_object));
    mylog::MyLog{}.info("load layer '{}' success;", layer_name);
}

void LevelLoader::loadTileLayer(const nlohmann::json& layer_json, Scene& scene) {
    // TODO;
}

void LevelLoader::loadObjectLayer(const nlohmann::json& layer_json, Scene& scene) {
    // TODO;
}

std::string LevelLoader::resolvePath(std::string image_path) {
    try {
        auto map_dir = std::filesystem::path(map_path_).parent_path();
        auto final_path = std::filesystem::canonical(map_dir / image_path);
        return final_path.string();
    } catch (const std::exception& e) {
        spdlog::error("resolvePath failed;image_path = {};error = {}", image_path, e.what());
        return "";
    }
}
} // scene
} // engine
