//
// Created by wangm on 26-10-12.
//

#ifndef RENDERER_H
#define RENDERER_H
#include <glm/vec2.hpp>
#include "sprite.h"

struct SDL_Renderer;
struct SDL_FRect;

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {
class Camera;

class Renderer final {
public:
    Renderer(SDL_Renderer* sdl_renderer, engine::resource::ResourceManager* resource_manager);
    ~Renderer() = default;
    Renderer(const Renderer& other) = delete;
    Renderer(Renderer&& other) noexcept = delete;
    Renderer& operator=(const Renderer& other) = delete;
    Renderer& operator=(Renderer&& other) noexcept = delete;

    /**
     * @brief 绘制精灵
     *
     * @param sprite 精灵对象；
     * @param position 精灵所在的世界坐标；
     * @param scale 缩放因子
     * @param angle 旋转角度；
     */
    void drawSprite(const engine::render::Camera& camera, const engine::render::Sprite& sprite,
                    const glm::vec2& position,
                    const glm::vec2& scale = {1.0f, 1.0f},
                    double angle = 0.0f);

    /**
     * @brief 绘制视差滚动背景
     *
     * @param camera 相机
     * @param sprite 要绘制的精灵对象
     * @param position 精灵所在的世界坐标
     * @param scroll_factor 滚动因子
     * @param repeat x y 轴 是否重复绘制
     * @param scale 缩放因子；
     */
    void drawParallax(const engine::render::Camera& camera, const engine::render::Sprite& sprite,
                      const glm::vec2& position,
                      const glm::vec2& scroll_factor,
                      const glm::bvec2& repeat = {true, true},
                      const glm::vec2& scale = {1.0f, 1.0f});

    /**
     * @brief 绘制ui sprite对象
     *
     * @param sprite 要绘制的精灵对象；
     * @param position ui对象在屏幕中的坐标
     * @param size 要绘制的 sprite 的大小， nullopt则为sprite原始大小；
     */
    void drawUISprite(const Sprite& sprite, const glm::vec2& position,
                      const std::optional<glm::vec2>& size = std::nullopt);

    void drawRect(const engine::render::Camera& camera, const glm::vec2& world_pos,const glm::vec2& size);
    void drawRect(const engine::render::Camera& camera, const glm::vec2& world_pos,const Sprite& sprite);

    void present(); // 更新屏幕
    void clearScreen(); // 清空屏幕
    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255); // 设置画笔颜色， 包装 SDL_SetRenderDrawColor 函数
    void setDrawColorFloat(float r, float g, float b, float a = 1.0f); // 设置画笔颜色， 包装 SDL_SetRenderDrawColorFloat 函数

    [[nodiscard]] SDL_Renderer* getSDLRenderer() const { return renderer_; }

private:
    SDL_Renderer* renderer_{nullptr};
    engine::resource::ResourceManager* resource_manager_{nullptr};

private:
    std::optional<SDL_FRect> getSpriteSrcRect(const engine::render::Sprite& sprite);
    bool isRectInViewport(const engine::render::Camera& camera,const SDL_FRect& rect);

};
} // render
// engine

#endif //RENDERER_H
