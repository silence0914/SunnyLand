//
// Created by wangm on 26-10-31.
//

#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H
#include <string>
#include <glm/vec2.hpp>
#include <SDL3/SDL_pixels.h>

namespace engine::render {
class Camera;
}

namespace engine::resource {
class ResourceManager;
}

struct SDL_Renderer;
struct TTF_TextEngine;

namespace engine {
namespace render {
/**
 * @brief 文本渲染
 */
class TextRenderer final {
public:
    TextRenderer(SDL_Renderer* renderer, engine::resource::ResourceManager& resource_manager);
    ~TextRenderer();

    TextRenderer(const TextRenderer& other) = delete;
    TextRenderer(TextRenderer&& other) noexcept = delete;
    TextRenderer& operator=(const TextRenderer& other) = delete;
    TextRenderer& operator=(TextRenderer&& other) noexcept = delete;

    void close();

    /**
     * @brief 绘制ui中的字符串
     * @param text utf-8 文本内容
     * @param font_id 字体id
     * @param font_size 字体大小
     * @param position 文本在屏幕的坐标（左上角）
     * @param color  文字颜色
     */
    void drawUiText(const std::string& text, const std::string& font_id, int font_size, const glm::vec2& position,
                    const SDL_FColor& color = {1.0f, 1.0f, 1.0f, 1.0f});

    /**
     * @brief 绘制地图上的字符串
     * @param camera 相机
     * @param text utf-8 文本
     * @param font_id 字体id
     * @param font_size 字体大小
     * @param position 文字坐标（左上角）
     * @param color 文本颜色
     */
    void drawText(const engine::render::Camera& camera, const std::string& text, const std::string& font_id,
                  int font_size, const glm::vec2& position, const SDL_FColor& color = {1.0f, 1.0f, 1.0f, 1.0f});

    /**
     * @brief 获取文本尺寸
     * @param text 要策略的文本
     * @param font_id 字体id
     * @param font_size 字体大小
     * @return 文本的尺寸
     */
    glm::vec2 getTextSize(const std::string& text, const std::string& font_id, int font_size);

private:
    SDL_Renderer* sdl_renderer_{nullptr};
    TTF_TextEngine* text_engine_{nullptr};
    engine::resource::ResourceManager& resource_manager_;
};
} // render
} // engine

#endif //TEXT_RENDERER_H
