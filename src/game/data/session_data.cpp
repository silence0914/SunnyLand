//
// Created by wangm on 26-10-30.
//

#include "session_data.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "../../engine/log/log_manager.h"
#include "../../engine/log/my_log.h"

namespace game {
namespace data {
void SessionData::setMaxHealth(int max_health) { max_health_ = std::max(1, max_health); }

void SessionData::addScore(int score) {
    current_score_ += score;
    if (current_health_ > high_score_) {
        setHighScore(current_score_);
    }
}

void SessionData::setHighScore(int high_score) { high_score_ = high_score; }

void SessionData::reset() {
    level_health_ = 3;
    level_score_ = 0;
    is_win = false;
    max_health_ = level_health_;
    current_health_ = max_health_;
    current_score_ = level_score_;
    map_path_ = "assets/maps/level1.tmj";
}

void SessionData::setNextLevel(const std::string& map_path) {
    map_path_ = map_path;
    level_health_ = current_health_;
    level_score_ = current_score_;
}

bool SessionData::saveToFile(const std::string& filename) const {
    nlohmann::json j;
    try {
        j["level_health"] = level_health_;
        j["level_score"] = level_score_;
        j["max_health"] = max_health_;
        j["high_score"] = high_score_;
        j["map_path"] = map_path_;

        std::ofstream ofs{filename};
        if (!ofs.is_open()) {
            mylog::MyLog{}.error("Failed to open file: {}", filename);
            return false;;
        }
        ofs << j.dump(4);
        return true;
    } catch (std::exception& e) {
        mylog::MyLog{}.error("Failed to save session data: {}", e.what());
        return false;
    }
}

bool SessionData::loadFromFile(const std::string& filename) {
    try {
        std::ifstream ifs{filename};
        if (!ifs.is_open()) {
            mylog::MyLog{}.error("Failed to open file: {}", filename);
            return false;
        }
        nlohmann::json j;
        ifs >> j;
        level_health_ = j.value("level_health", 3);
        level_score_ = j.value("level_score", 0);
        current_score_ = level_score_;
        max_health_ = j.value("max_health", 3);
        current_health_ = max_health_;
        high_score_ = std::max(j.value("high_score", 0), high_score_);
        map_path_ = j.value("map_path", "assets/maps/level1.tmj");
        return true;
    } catch (std::exception& e) {
        mylog::MyLog{}.error("Failed to load session data: {}", e.what());
        return false;
    }
}

bool SessionData::syncHighScore(const std::string& filename) {
    try {
        // 打开存档文件
        std::fstream fs{filename};
        if (!fs.is_open()) {
            mylog::MyLog{}.warn("Failed to open file: {}", filename);
            return false;
        }
        nlohmann::json j;
        fs >> j;
        // 解析存档；
        auto high_score_in_file = j.value("high_score", 0);
        if (high_score_in_file < high_score_) {
            j["high_score"] = high_score_; // 设置最高分；
            fs.seekp(0);                   // 回到文件开头；
            fs << j.dump(4);
        } else {
            high_score_ = high_score_in_file;
        }
        return true;
    } catch (const std::exception& e) {
        mylog::MyLog{}.error("Failed to sync high score {} : {}", filename, e.what());
        return false;
    }
}
} // data
} // game
