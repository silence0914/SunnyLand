//
// Created by wangm on 26-10-12.
//

#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include <optional>
#include <utility>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

namespace engine::render {
class Sprite {
public:
    Sprite() = default;
    /**
     *
     * @param texture_id 纹理id
     * @param source_rect 纹理需要绘制的矩形区域 SDL_FRect； 如果为std::nullopt 使用整个纹理；
     * @param is_flipped 是否水平翻转  默认不翻转；
     */
    Sprite(std::string texture_id, const std::optional<SDL_FRect>& source_rect = std::nullopt,
           bool is_flipped = false): texture_id_(std::move(texture_id)), source_rect_(source_rect),
                                     is_flipped_(is_flipped) {}

    [[nodiscard]] std::string getTextureId() const { return texture_id_; }
    void setTextureId(const std::string& texture_id) { texture_id_ = texture_id; }

    [[nodiscard]] std::optional<SDL_FRect> getSourceRect() const { return source_rect_; }
    void setSourceRect(const std::optional<SDL_FRect>& source_rect) { source_rect_ = source_rect; }

    [[nodiscard]] bool isIsFlipped() const { return is_flipped_; }
    void setIsFlipped(bool is_flipped) { is_flipped_ = is_flipped; }

private:
    std::string texture_id_;               // 纹理资源id；
    std::optional<SDL_FRect> source_rect_; // 要绘制的纹理区域
    bool is_flipped_{false};               // 是否水平反转
};
} // render
// engine

#endif //SPRITE_H
