//
// Created by wangm on 26-10-8.
//

#ifndef TIME_H
#define TIME_H

#include <chrono>

namespace engine::core {
using duration_unit = std::chrono::milliseconds;

class Time final {
public:
    Time(int target_fps = 0);
    ~Time() = default;
    Time(Time&) = delete;
    Time(Time&&) = delete;
    Time& operator=(Time&) = delete;
    Time& operator=(Time&&) = delete;

    void update();

    // 设置帧数；
    void setTargetFps(const int target_fps = 0) {
        target_fps_ = target_fps;
        setTargetFrameTime();
    };

    void setTimeScale(double time_scale) {
        time_scale_ = time_scale;
    }

    [[nodiscard]] duration_unit getDeltaTime() const {
        return std::chrono::duration_cast<duration_unit>(delta_time_ * time_scale_);
    }

    [[nodiscard]] int getCurrentFps() const {
        return current_fps_;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> last_time_{}; // 上一帧开始的时间；
    duration_unit delta_time_{0};                                    // 上一帧开始到这一帧开始的时间差；
    double time_scale_{1.0};                                         // 时间缩放因子；
    int target_fps_{0};                                              // 目标fps， 0无限制
    duration_unit target_frame_time_{};                              // 目标每帧时间；

    int current_fps_{0};                                                        // 实际帧数；
    std::chrono::time_point<std::chrono::steady_clock> last_count_frame_ago_{}; // 30帧前的时间点
private:
    void setTargetFrameTime();
    void calculateCurrentFps(std::chrono::time_point<std::chrono::steady_clock>& start_time);
};
}

#endif //TIME_H
