//
// Created by wangm on 26-10-25.
//

#include "animation.h"

#include <glm/common.hpp>

#include "../log/log_manager.h"
#include "../log/my_log.h"

namespace engine {
namespace render {
Animation::Animation(const std::string& name, bool loop) : name_(name), loop_(loop) {}

void Animation::addFrame(const SDL_FRect& source_rect, float duration) {
    if (duration <= 0.0f) {
        mylog::MyLog{}.warn("'{}' addFrame failed; duration <= 0.0f", name_);
        return;
    }
    frames_.emplace_back(source_rect, duration);
    total_duration += duration;
}

const AnimationFrame& Animation::getFrame(float time) const {
    if (frames_.empty()) {
        mylog::MyLog{}.error("'{}' getFrame failed; animation is empty", name_);
        return frames_.back();
    }
    if (total_duration <= 0) {
        mylog::MyLog{}.error("'{}' getFrame failed; total_duration <= 0", name_);
        return frames_.back();
    }

    float current_time = time;
    if (loop_) {
        current_time = glm::mod(time, total_duration);
    } else {
        // 非循环动画   时间超出总长， 返回最后一帧；
        if (current_time >= total_duration) {
            return frames_.back();
        }
    }

    // 遍历每相加每一帧的时间，  获取正确帧；
    float accumulated_time = 0.0f;
    for (auto& frame: frames_) {
        accumulated_time += frame.duration;
        if (accumulated_time > current_time) {
            return frame;
        }
    }

    mylog::MyLog{}.warn("animation '{}' getFrame error;", name_);
    return frames_.back();
}
} // render
} // engine
