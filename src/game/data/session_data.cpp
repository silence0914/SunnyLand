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
        high_score_ = j.value("high_score", 0);
        map_path_ = j.value("map_path", "assets/maps/level1.tmj");
        return true;
    } catch (std::exception& e) {
        mylog::MyLog{}.error("Failed to load session data: {}", e.what());
        return false;
    }
}
} // data
} // game
