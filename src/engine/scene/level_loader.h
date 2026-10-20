//
// Created by wangm on 26-10-19.
//

#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H
#include <string>
#include <nlohmann/json.hpp>
#include "../component/component.h"
#include <map>
#include <glm/vec2.hpp>

namespace engine::component {
struct TileInfo;
}

namespace engine::scene {
class Scene;

class LevelLoader final {
public:
    LevelLoader() = default;
    ~LevelLoader() = default;;

    bool loadLevel(const std::string& map_path, Scene& scene);

private:
    void loadImageLayer(const nlohmann::json& layer_json, Scene& scene);
    void loadTileLayer(const nlohmann::json& layer_json, Scene& scene);
    void loadObjectLayer(const nlohmann::json& layer_json, Scene& scene);

    /**
     * @brief 根据全局id获取瓦片信息
     * @param gid 全局id
     * @return 瓦片信息
     */
    engine::component::TileInfo getTileInfoByGid(int gid);

    /**
     * @brief 加载 Tiled tileset 文件 (.tsj)
     * @param tileset_path tileset 文件路径
     * @param first_gid tileset 的第一个全局id
     */
    void loadTileset(const std::string& tileset_path, int first_gid);

    /**
     * @brief 解析图片路径，合并地图路径和相对路径
     * @param relative_path 地图相对路径；
     * @param file_path 地图文件路径
     * @return 解析后的完整路径；
     */
    std::string resolvePath(const std::string& relative_path, const std::string& file_path);

private:
    std::string map_path_{};                     // 地图路径
    glm::ivec2 map_size_{};                      // 地图尺寸
    glm::ivec2 tile_size_{};                     // 瓦片尺寸
    std::map<int, nlohmann::json> tileset_data_; // 瓦片数据
};
} // scene
// engine

#endif //LEVEL_LOADER_H
