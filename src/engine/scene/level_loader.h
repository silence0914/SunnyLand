//
// Created by wangm on 26-10-19.
//

#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H
#include <string>
#include <nlohmann/json_fwd.hpp>

namespace engine::scene {
class Scene;

class LevelLoader final {
public:
    LevelLoader() = default;
    ~LevelLoader() = default;;

    bool loadLevel(const std::string& map_path, Scene& scene);

private:
    std::string map_path_{}; // 地图路径

private:
    void loadImageLayer(const nlohmann::json& layer_json, Scene& scene);
    void loadTileLayer(const nlohmann::json& layer_json, Scene& scene);
    void loadObjectLayer(const nlohmann::json& layer_json, Scene& scene);

    /**
     * @brief 解析图片路径，合并地图路径和相对路径
     * @param image_path 地图相对路径；
     * @return 解析后的完整路径；
     */
    std::string resolvePath(std::string image_path);


};
} // scene
// engine

#endif //LEVEL_LOADER_H
