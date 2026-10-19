//
// Created by wangm on 26-10-13.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <string>

#include "../utils/action.h"


namespace engine::core {
class Config final {
public:
    // 默认配置：
    // 窗口设置
    std::string window_title_ = "SunnyLand";
    int window_width_ = 1280;
    int window_height_ = 720;
    bool window_resizable_ = true;

    // 图形设置
    bool vsync_enabled_ = true; // 是否垂直同步；
    int target_fps_ = 60;

    // 音频设置
    float music_volume_ = 0.5f;
    float sound_volume_ = 0.5f;

    // 按键 - 动作映射
    std::unordered_map<std::string, std::vector<std::string> > input_mappings_{
        {std::string(engine::utils::action::MOVE_LEFT), {"A", "Left"}},
        {std::string(engine::utils::action::MOVE_RIGHT), {"D", "Right"}},
        {std::string(engine::utils::action::MOVE_UP), {"W", "Up"}},
        {std::string(engine::utils::action::MOVE_DOWN), {"S", "Down"}},
        {std::string(engine::utils::action::JUMP), {"J", "Space"}},
        {std::string(engine::utils::action::ATTACK), {"K", "MouseLeft"}},
        {std::string(engine::utils::action::PAUSE), {"P", "Escape"}}
    };

    explicit Config(const std::string& filepath);

    Config(const Config& other) = delete;
    Config(Config&& other) noexcept = delete;
    Config& operator=(const Config& other) = delete;
    Config& operator=(Config&& other) noexcept = delete;

    bool loadFromFile(const std::string& filepath = "");
    bool saveToFile(const std::string& filepath = "");

private:
    std::string config_file_path_;
    void fromJson(const nlohmann::json& j);
    nlohmann::ordered_json toJson() const;
};
} // core
// engine

#endif //CONFIG_H
