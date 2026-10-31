//
// Created by wangm on 26-10-31.
//

#include "text_renderer.h"

#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>

#include "camera.h"
#include "../log/my_log.h"
#include "../resource/resource_manager.h"

namespace engine {
namespace render {
TextRenderer::TextRenderer(SDL_Renderer* renderer, engine::resource::ResourceManager& resource_manager)
    : sdl_renderer_(renderer), resource_manager_(resource_manager) {
    if (!sdl_renderer_) {
        throw std::runtime_error("SDL_Renderer is null");
    }
    if (!TTF_WasInit() && !TTF_Init()) {
        throw std::runtime_error("Failed to initialize SDL_ttf: " + std::string(SDL_GetError()));
    }
    text_engine_ = TTF_CreateRendererTextEngine(sdl_renderer_);
    if (!text_engine_) {
        mylog::MyLog{}.error("Failed to create SDL_ttf text engine: " + std::string(SDL_GetError()));
        throw std::runtime_error("Failed to create SDL_ttf text engine");
    }
    mylog::MyLog{}.trace("create TextRender success");
}

TextRenderer::~TextRenderer() { close(); }

void TextRenderer::close() {
    if (text_engine_) {
        TTF_DestroyRendererTextEngine(text_engine_);
        text_engine_ = nullptr;
        mylog::MyLog{}.trace("destroy textEngine");
    }
    TTF_Quit();
}

void TextRenderer::drawUiText(const std::string& text, const std::string& font_id, int font_size,
                              const glm::vec2& position, const SDL_FColor& color) {
    // 获取字体
    TTF_Font* font = resource_manager_.getFont(font_id, font_size);
    if (!font) {
        mylog::MyLog{}.warn("Failed to get font '{}' , fontsize '{}'", font_id, font_size);
        return;
    }

    // 创造临时TTF_Text对象
    TTF_Text* temp_text_object = TTF_CreateText(text_engine_, font, text.c_str(), 0);
    if (!temp_text_object) {
        mylog::MyLog{}.warn("Failed to create TTF_Text object: {}", SDL_GetError());
        return;
    }

    // 绘制文字阴影
    TTF_SetTextColorFloat(temp_text_object, 0.0f, 0.0f, 0.0f, 1.0f);
    if (!TTF_DrawRendererText(temp_text_object, position.x + 2, position.y + 2)) {
        mylog::MyLog{}.warn("Failed to draw black TTF_Text object: {}", SDL_GetError());
    }

    TTF_SetTextColorFloat(temp_text_object, color.r, color.g, color.b, color.a);
    if (!TTF_DrawRendererText(temp_text_object, position.x, position.y)) {
        mylog::MyLog{}.warn("Failed to draw b TTF_Text object: {}", SDL_GetError());
    }

    TTF_DestroyText(temp_text_object);
}

void TextRenderer::drawText(const engine::render::Camera& camera, const std::string& text, const std::string& font_id,
                            int font_size, const glm::vec2& position, const SDL_FColor& color) {
    glm::vec2 position_screen = camera.worldToScreen(position);
    drawUiText(text, font_id, font_size, position_screen, color);
}

glm::vec2 TextRenderer::getTextSize(const std::string& text, const std::string& font_id, int font_size) {
    TTF_Font* font = resource_manager_.getFont(font_id, font_size);
    if (!font) {
        mylog::MyLog{}.warn("Failed to get font '{}' , fontsize '{}'", font_id, font_size);
        return glm::vec2{0.0f};
    }
    TTF_Text* temp_text_obj = TTF_CreateText(text_engine_, font, text.c_str(), 0);
    if (!temp_text_obj) {
        mylog::MyLog{}.warn("Failed to create TTF_Text object: {}", SDL_GetError());
        return glm::vec2{0.0f};
    }
    int width, height;
    TTF_GetTextSize(temp_text_obj, &width, &height);
    TTF_DestroyText(temp_text_obj);
    return glm::vec2{static_cast<float>(width) , static_cast<float>(height)};
}
} // render
} // engine
