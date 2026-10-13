//
// Created by wangm on 26-10-13.
//
#include <fstream>
#include "config.h"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace engine {
namespace core {
Config::Config(const std::string& filepath) : config_file_path_(filepath) {
    loadFromFile(filepath);
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        spdlog::warn("config '{}' not have;");
        if (!saveToFile(filepath)) {
            spdlog::error("create config failed; filepath : {} ", filepath);
        }
        return false; // 配置文件不存在；  使用默认参数；
    }
    try {
        nlohmann::json j;
        file >> j;
        fromJson(j);
        spdlog::info("loadFromFile success");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("loadFromFile failed; parse file failed; filepath = {}, error = {}", filepath, e.what());
    }
    return false;
}

bool Config::saveToFile(const std::string& filepath) {
    std::ofstream file{filepath};
    if (!file.is_open()) {
        spdlog::error("saveToFile failed; open config file failed;filepath = {}", filepath);
        return false;
    }

    try {
        nlohmann::ordered_json j = toJson();
        file << j.dump(4);
        spdlog::info("config saveToFile completion");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("saveToFile failed; write config to file failed; filepath = {}, error = {}", filepath, e.what());
    }
    return false;
}

void Config::fromJson(const nlohmann::json& j) {
    if (j.contains("window")) {
        const auto& window_config = j["window"];
        window_title_ = window_config.value("title", window_title_);
        window_width_ = window_config.value("width", window_width_);
        window_height_ = window_config.value("height", window_height_);
        window_resizable_ = window_config.value("resizable", window_resizable_);
    } else {
        spdlog::warn("fromJson, '{}' not exit", "window");
    }
    if (j.contains("graphics")) {
        const auto& graphics_config = j["graphics"];
        vsync_enabled_ = graphics_config.value("vsync", vsync_enabled_);
    } else {
        spdlog::warn("fromJson, '{}' not exit", "graphics");
    }
    if (j.contains("performance")) {
        const auto& perf_config = j["performance"];
        target_fps_ = perf_config.value("target_fps", target_fps_);
        if (target_fps_ < 0) {
            target_fps_ = 0;
        }
    } else {
        spdlog::warn("fromJson, '{}' not exit", "performance");
    }
    if (j.contains("audio")) {
        const auto& audio_config = j["audio"];
        music_volume_ = audio_config.value("music_volume", music_volume_);
        sound_volume_ = audio_config.value("sound_volume", sound_volume_);
    } else {
        spdlog::warn("fromJson, '{}' not exit", "audio");
    }

    if (j.contains("input_mapping") && j["input_mapping"].is_object()) {
        const auto& mappings_json = j["input_mappings"];
        try {
            auto input_mapping = mappings_json.get<std::unordered_map<std::string, std::vector<std::string> > >();
            input_mappings_ = std::move(input_mapping);
            spdlog::trace("config formJson completion");
        } catch (const std::exception& e) {
            spdlog::warn("formJson failed;analysis 'input_mapping' exception , error = {}", e.what());
        }
    } else {
        spdlog::warn("fromJson, '{}' not exit", "input_mapping");
    }
}

nlohmann::ordered_json Config::toJson() const {
    return nlohmann::ordered_json{
        {
            "window", {
                {"title", window_title_},
                {"width", window_width_},
                {"height", window_height_},
                {"resizable", window_resizable_}
            }
        },
        {
            "graphics", {
                {"vsync", vsync_enabled_}
            }
        },
        {
            "performance", {
                {"target_fps", target_fps_}
            }
        },
        {
            "audio", {
                {"music_volume", music_volume_},
                {"sound_volume", sound_volume_}
            }
        },
        {"input_mappings", input_mappings_}
    };
}
} // core
} // engine
