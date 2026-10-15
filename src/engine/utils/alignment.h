//
// Created by wangm on 26-10-14.
//

#ifndef ALIGNMENT_H
#define ALIGNMENT_H

namespace engine::utils {
enum class Alignment {
    NONE, // 不指定
    TOP_LEFT, // 上左
    TOP_CENTER, // 上中
    TOP_RIGHT, // 上右
    CENTER_LEFT, // 中左
    CENTER, // 中心
    CENTER_RIGHT, // 中右
    BOTTOM_LEFT, // 下左
    BOTTOM_CENTER, // 下中
    BOTTOM_RIGHT // 下右
};
}

#endif //ALIGNMENT_H
