//
// Created by wangm on 26-10-30.
//

#ifndef SESSION_DATA_H
#define SESSION_DATA_H
#include <string>

namespace game {
namespace data {
class SessionData final {
public:
    SessionData() = default;
    ~SessionData() = default;

    SessionData(const SessionData& other) = delete;
    SessionData(SessionData&& other) noexcept = delete;
    SessionData& operator=(const SessionData& other) = delete;
    SessionData& operator=(SessionData&& other) noexcept = delete;

    [[nodiscard]] int getCurrentHealth() const { return current_health_; }
    [[nodiscard]] int getMaxHealth() const { return max_health_; }
    [[nodiscard]] int getCurrentScore() const { return current_score_; }
    [[nodiscard]] int getHighScore() const { return high_score_; }
    [[nodiscard]] int getLevelHealth() const { return level_health_; }
    [[nodiscard]] int getLevelScore() const { return level_score_; }
    [[nodiscard]] std::string getMapPath() const { return map_path_; }

    void setCurrentHealth(int current_health) { current_health_ = current_health; }
    void setMaxHealth(int max_health);
    void addScore(int score);
    void setHighScore(int high_score);
    void setLevelHealth(int level_health) { level_health_ = level_health; }
    void setLevelScore(int level_score) { level_score_ = level_score; }
    void setMapPath(const std::string& map_path) { map_path_ = map_path; }

    void reset();                                       // 重置游戏数据
    void setNextLevel(const std::string& map_path);     // 设置下一个场景信息
    bool saveToFile(const std::string& filename) const; // 将当前游戏数据保存到文件
    bool loadFromFile(const std::string& filename);     // 从文件中读取游戏数据；

private:
    int current_health_ = 3;
    int max_health_ = 3;
    int current_score_ = 0;
    int high_score_ = 0;

    int level_health_ = 3; // 进入关卡时最大血量
    int level_score_ = 0; // 进入关卡是的得分
    std::string map_path_ = "assets/maps/level1.tmj";

};
} // data
} // game

#endif //SESSION_DATA_H
