//
// Created by wangm on 26-10-12.
//

#include "renderer.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

#include "camera.h"
#include "../resource/resource_manager.h"

namespace engine {
namespace render {
Renderer::Renderer(SDL_Renderer* sdl_renderer, engine::resource::ResourceManager* resource_manager)
    : renderer_(sdl_renderer), resource_manager_(resource_manager) {
    spdlog::trace("create Renderer start");
    if (renderer_ == nullptr) {
        spdlog::error("create Renderer failed,renderer_ == nullptr");
        throw std::runtime_error("construction Renderer failed;renderer_ == nullptr");
    }
    if (resource_manager_ == nullptr) {
        spdlog::error("create Renderer failed,resource_manager_ == nullptr");
        throw std::runtime_error("construction Renderer failed;resource_manager_ == nullptr");
    }
    setDrawColor(0, 0, 0);
    clearScreen();
    spdlog::trace("create Renderer completion");
}

void Renderer::drawSprite(const Camera& camera, const Sprite& sprite, const glm::vec2& position, const glm::vec2& scale,
                          double angle) {
    auto texture = resource_manager_->getTexture(sprite.getTextureId());
    if (texture == nullptr) {
        spdlog::error("drawSprite failed : getTexture failed; texture id = {}", sprite.getTextureId());
        return;
    }
    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value()) {
        spdlog::error("drawSprite failed : getSpriteSrcRect failed;texture_id = {}", sprite.getTextureId());
        return;
    }
    // 获取要绘制的屏幕坐标位置；
    glm::vec2 position_screen = camera.worldToScreen(position);

    // 显示尺寸缩放；
    float scaled_w = src_rect.value().w * scale.x;
    float scaled_h = src_rect.value().h * scale.y;

    SDL_FRect dest_rect{
        position_screen.x,
        position_screen.y,
        scaled_w,
        scaled_h
    };

    if (!isRectInViewport(camera, dest_rect)) {
        // 超出视窗， 不用绘制；
        return;
    }

    if (!SDL_RenderTextureRotated(renderer_, texture,
                                  &src_rect.value(),
                                  &dest_rect,
                                  angle,
                                  nullptr,
                                  sprite.isIsFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("drawSprite failed; SDL_RenderTextureRotated failed;texture_id = {}; error = {}",
                      sprite.getTextureId(), SDL_GetError());
    }
}

void Renderer::drawParallax(const engine::render::Camera& camera, const Sprite& sprite, const glm::vec2& position,
                            const glm::vec2& scroll_factor, const glm::bvec2& repeat, const glm::vec2& scale) {
    auto texture = resource_manager_->getTexture(sprite.getTextureId());
    if (texture == nullptr) {
        spdlog::error("drawParallax failed : getTexture failed; texture id = {}", sprite.getTextureId());
        return;
    }
    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value()) {
        spdlog::error("drawParallax failed : getSpriteSrcRect failed;texture_id = {}", sprite.getTextureId());
        return;
    }
    // 获取要绘制的屏幕坐标位置；
    glm::vec2 position_screen = camera.worldToScreenWithParallax(position, scroll_factor);

    // 显示尺寸缩放；
    float scaled_tex_w = src_rect.value().w * scale.x;
    float scaled_tex_h = src_rect.value().h * scale.y;

    glm::vec2 start{}, stop{};
    glm::vec2 viewport_size = camera.getViewportSize();

    if (repeat.x) {
        start.x = glm::mod(position_screen.x, scaled_tex_w) - scaled_tex_w;
        stop.x = viewport_size.x;
    } else {
        start.x = position_screen.x;
        stop.x = glm::min(position_screen.x + scaled_tex_w, viewport_size.x);
    }

    if (repeat.y) {
        start.y = glm::mod(position_screen.y, scaled_tex_h) - scaled_tex_h;
        stop.y = viewport_size.y;
    } else {
        start.y = position_screen.y;
        stop.y = glm::min(position_screen.y + scaled_tex_h, viewport_size.y);
    }

    for (float y = start.y; y < stop.y; y += scaled_tex_h) {
        for (float x = start.x; x < stop.x; x += scaled_tex_w) {
            SDL_FRect dst_rect{x, y, scaled_tex_w, scaled_tex_h};
            if (!SDL_RenderTexture(renderer_, texture, &src_rect.value(), &dst_rect)) {
                spdlog::error("drawParallax failed : SDL_RenderTexture failed;texture_id = {}; sdl_error = {}",
                              sprite.getTextureId(), SDL_GetError());
                return;
            }
        }
    }
}

void Renderer::drawUISprite(const Sprite& sprite, const glm::vec2& position, const std::optional<glm::vec2>& size) {
    auto texture = resource_manager_->getTexture(sprite.getTextureId());
    if (texture == nullptr) {
        spdlog::error("drawUISprite failed : getTexture failed; texture id = {}", sprite.getTextureId());
        return;
    }
    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value()) {
        spdlog::error("drawUISprite failed : getSpriteSrcRect failed;texture_id = {}", sprite.getTextureId());
        return;
    }
    SDL_FRect dst_rect{position.x, position.y, src_rect.value().w, src_rect.value().h};
    if (size.has_value()) {
        dst_rect.w = size.value().y;
        dst_rect.h = size.value().y;
    }
    if (!SDL_RenderTextureRotated(renderer_, texture, &src_rect.value(), &dst_rect, 0.0, nullptr,
                                  sprite.isIsFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("drawUISprite failed : SDL_RenderTextureRotated failed; texture id = {},sdl_error = {}",
                      sprite.getTextureId(), SDL_GetError());
    }
}

void Renderer::drawRect(const engine::render::Camera& camera, const glm::vec2& world_pos, const glm::vec2& size) {
    auto pos = camera.worldToScreen(world_pos);
    SDL_FRect rect{pos.x, pos.y, size.x, size.y};
    if (!SDL_RenderRect(renderer_, &rect)) {
        spdlog::error("drawCollider failed; SDL_RenderRect failed; sdl_error = {}", SDL_GetError());
    }
}

void Renderer::drawRect(const engine::render::Camera& camera, const glm::vec2& world_pos, const Sprite& sprite) {
    auto pos = camera.worldToScreen(world_pos);
    auto size = getSpriteSrcRect(sprite);
    SDL_FRect rect{pos.x, pos.y, size->w, size->h};
    if (!SDL_RenderRect(renderer_, &rect)) {
        spdlog::error("drawCollider failed; SDL_RenderRect failed; sdl_error = {}", SDL_GetError());
    }
}

void Renderer::present() {
    if (!SDL_RenderPresent(renderer_)) {
        spdlog::error("present failed; sdl_error = {}", SDL_GetError());
    }
}

void Renderer::clearScreen() {
    if (!SDL_RenderClear(renderer_)) {
        spdlog::error("clearScreen failed; sdl_error = {}", SDL_GetError());
    }
}

void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!SDL_SetRenderDrawColor(renderer_, r, g, b, a)) {
        spdlog::error("setDrawColor failed; sdl_error = {}", SDL_GetError());
    }
}

void Renderer::setDrawColorFloat(float r, float g, float b, float a) {
    if (!SDL_SetRenderDrawColorFloat(renderer_, r, g, b, a)) {
        spdlog::error("setDrawColorFloat failed; sdl_error = {}", SDL_GetError());
    }
}

std::optional<SDL_FRect> Renderer::getSpriteSrcRect(const engine::render::Sprite& sprite) {
    SDL_Texture* texture = resource_manager_->getTexture(sprite.getTextureId());
    if (!texture) {
        spdlog::error("getSpriteSrcRect failed: getTexture Failed;texture_id = {}", sprite.getTextureId());
        return std::nullopt;
    }

    if (auto src_rect = sprite.getSourceRect(); src_rect.has_value()) {
        if (src_rect.value().w <= 0 || src_rect.value().h <= 0) {
            spdlog::error("getSpriteSrcRect failed; source rect invalid,texture_id = {}, w = {},h = {}",
                          sprite.getTextureId(), src_rect->w, src_rect->h);
            return std::nullopt;
        }
        return src_rect;
    }
    SDL_FRect result{0, 0, 0, 0};
    if (!SDL_GetTextureSize(texture, &result.w, &result.h)) {
        spdlog::error("getSpriteSrcRect failed; GetTextureSize failed;texture_id = {}; sdl_error : {}",
                      sprite.getTextureId(), SDL_GetError());
        return std::nullopt;
    }
    return result;
}

bool Renderer::isRectInViewport(const engine::render::Camera& camera, const SDL_FRect& rect) {
    glm::vec2 viewport_size = camera.getViewportSize();
    return rect.x + rect.w >= 0 &&
            rect.y + rect.h >= 0 &&
            rect.x <= viewport_size.x &&
            rect.y <= viewport_size.y;
}
} // render
} // engine
