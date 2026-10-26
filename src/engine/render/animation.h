//
// Created by wangm on 26-10-25.
//

#ifndef ANIMATION_H
#define ANIMATION_H
#include <SDL3/SDL_rect.h>
#include <string>
#include <vector>

namespace engine {
namespace render {
struct AnimationFrame {
    SDL_FRect source_rect; // 纹理集上 此帧的区域
    float duration;        // 此帧的持续时间；
};

/**
 * @brief 管理动画帧
 *
 * 储存动画的帧，总时长，名称，循环行为
 */
class Animation final {
public:
    /**
     * @brief 构造函数
     * @param name 动画名称
     * @param loop 动画是否循环播放
     */
    Animation(const std::string& name = "default", bool loop = true);
    ~Animation() = default;

    Animation(const Animation& other) = delete;
    Animation(Animation&& other) noexcept = delete;
    Animation& operator=(const Animation& other) = delete;
    Animation& operator=(Animation&& other) noexcept = delete;

    /**
     * @brief 动画加一帧；
     * @param source_rect 纹理集上 此帧的区域
     * @param duration 此帧的持续时间；
     */
    void addFrame(const SDL_FRect& source_rect, float duration);

    /**
     * @brief 获取指定时间点应该显示的动画帧
     * @param time 当前时间（秒）； 如果是循环动画，可以超过总持续时间；
     * @return 对应时间点的动画帧
     */
    const AnimationFrame& getFrame(float time) const;

    // get/set
    [[nodiscard]] const std::string& getName() const { return name_; }                     // 获取动画名称
    [[nodiscard]] const std::vector<AnimationFrame>& getFrames() const { return frames_; } // 获取所有动画帧
    [[nodiscard]] float getTotalDuration() const { return total_duration; }                // 获取动画总时长
    [[nodiscard]] size_t getFrameCount() const { return frames_.size(); }                  // 获取动画帧数
    [[nodiscard]] bool isLoop() const { return loop_; }                                    // 获取动画是否循环播放
    [[nodiscard]] bool isEmpty() const { return frames_.empty(); }                         // 获取动画是否为空

    void setName(const std::string& name) { name_ = name; } // 设置动画名称
    void setLoop(bool loop) { loop_ = loop; }               // 设置动画是否循环播放

private:
    std::string name_;
    std::vector<AnimationFrame> frames_;
    float total_duration = 0.0f; // 动画的总持续时间；
    bool loop_ = true;
};
} // render
} // engine

#endif //ANIMATION_H
