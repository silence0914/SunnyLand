//
// Created by wangm on 26-10-8.
//

#include "time.h"
#include <thread>
#include <spdlog/spdlog.h>
#include <iostream>

namespace engine::core {
Time::Time(int target_fps) {
    last_time_ = std::chrono::steady_clock::now();
    last_count_frame_ago_ = last_time_;
    target_fps_ = target_fps;
    setTargetFrameTime();
}

void Time::update() {
    // 限制帧数，  睡眠到预设时间点唤醒；
    if (target_fps_ != 0) {
        std::this_thread::sleep_until(last_time_ + target_frame_time_);
    }
    auto start_time = std::chrono::steady_clock::now();
    delta_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(start_time - last_time_);
    last_time_ = start_time;

    calculateCurrentFps(start_time);
}

void Time::setTargetFrameTime() {
    if (target_fps_ == 0) { return; }
    target_frame_time_ = std::chrono::milliseconds(1000) / target_fps_;
    spdlog::debug("target_frame_time = {}", target_frame_time_.count());
}

void Time::calculateCurrentFps(std::chrono::time_point<std::chrono::steady_clock>& start_time) {
    constexpr int count_frequency = 60;
    static int count = 0;

    if (count++ < count_frequency) { return; }

    count = 0;
    current_fps_ = (std::chrono::milliseconds(1000) * count_frequency) / (start_time - last_count_frame_ago_) + 1;

    last_count_frame_ago_ = start_time;

    spdlog::debug("fps = {}", current_fps_);
}
}
