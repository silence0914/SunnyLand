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

#include "../../game/component/ai_component.h"
#include "../utils/math.h"

namespace engine::component {
class AnimationComponent;
class AudioComponent;
struct TileInfo;
enum class TileType;
}

namespace engine::scene {
class Scene;

class LevelLoader final {
public:
    LevelLoader() = default;
    ~LevelLoader() = default;;

    bool loadLevel(const std::string& map_path, Scene& scene);

private:
    void loadImageLayer(const nlohmann::json& layer_json, Scene& scene);  // 加载图片层
    void loadTileLayer(const nlohmann::json& layer_json, Scene& scene);   // 加载瓦片层
    void loadObjectLayer(const nlohmann::json& layer_json, Scene& scene); // 加载对象层

    /**
     * @brief 添加动画到指定的AnimationComponent;
     * @param anim_json 动画的json数据；
     * @param ac AnimationComponent 指针；
     * @param sprite_size 每帧动画的尺寸；
     */
    void addAnimation(const nlohmann::json& anim_json, engine::component::AnimationComponent& ac,
                      const glm::vec2& sprite_size);

    /**
     * @brief 获取瓦片属性；
     * @tparam T 属性类型
     * @param tile_json 瓦片json数据
     * @param property_name 属性名称
     * @return 属性值， 如果不存在返回std::nullopt;
     */
    template<typename T>
    std::optional<T> getTileProperty(const nlohmann::json& tile_json, const std::string& property_name) {
        if (!tile_json.contains("properties")) { return std::nullopt; }
        const auto& properties = tile_json["properties"];
        for (const auto& property: properties) {
            if (property.contains("name") && property["name"] == property_name) {
                if (property.contains("value")) {
                    return property["value"].get<T>();
                }
            }
        }
        return std::nullopt;
    }

    /**
     * @brief 获取瓦片碰撞矩形
     * @param tile_json 瓦片json数据
     * @return 碰撞器矩形  如果不存在碰撞器 返回std::nullopt;
     */
    std::optional<engine::utils::Rect> getColliderRect(const nlohmann::json& tile_json);

    /**
     * @brief 根据全局id获取 瓦片json对象；
     * @param gid 全局id
     * @return 瓦片json对象；
     */
    std::optional<nlohmann::json> getTileJsonByGid(int gid) const;

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

    /**
     * @brief 更具瓦片json对象获取瓦片类型；
     * @param tile_json 瓦片json数据
     * @return 瓦片类型
     */
    engine::component::TileType getTileType(const nlohmann::json& tile_json);

    /**
     * @brief 根据图块集中的id获取瓦片类型；
     * @param tileset_json 图块集中的json数据
     * @param local_id 图块集中的id；
     * @return 瓦片类型
     */
    engine::component::TileType getTileTypeById(const nlohmann::json& tileset_json, int local_id);

    /**
     * @brief 添加音效到指定的 audioComponent
     * @param sound_json 音效json数据
     * @param audio_component  AudioComponent对象
     */
    void addSound(const nlohmann::json& sound_json, engine::component::AudioComponent& audio_component);

private:
    std::string map_path_{};                     // 地图路径
    glm::ivec2 map_size_{};                      // 地图尺寸
    glm::ivec2 tile_size_{};                     // 瓦片尺寸
    std::map<int, nlohmann::json> tileset_data_; // 瓦片数据
};
} // scene
// engine

#endif //LEVEL_LOADER_H
