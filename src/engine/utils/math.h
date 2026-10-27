//
// Created by wangm on 26-10-12.
//

#ifndef MATH_H
#define MATH_H

#include <glm/glm.hpp>
#include <chrono>

namespace engine::utils {
struct Rect {
    glm::vec2 position;
    glm::vec2 size;

    glm::vec2 getCenter() { return position + size / 2.0f; }
};
}

#endif //MATH_H
