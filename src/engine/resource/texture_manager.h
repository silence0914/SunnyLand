//
// Created by wangm on 26-10-11.
//

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <glm/vec2.hpp>
#include <SDL3/SDL_render.h>

namespace engine::resource {
class TextureManager final {
    friend class ResourceManager;

public:
    explicit TextureManager(SDL_Renderer* renderer);
    ~TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager(TextureManager&&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager& operator=(TextureManager&&) = delete;

private:
    SDL_Texture* loadTexture(const std::string& file_path); // 加载纹理
    SDL_Texture* getTexture(const std::string& file_path); // 获取纹理
    void unloadTexture(const std::string& file_path); // 卸载纹理
    glm::vec2 getTextureSize(const std::string& file_path); // 获取纹理尺寸；
    void clearTextures(); //清空纹理

private:
    struct SDLTextureDelete {
        void operator()(SDL_Texture* texture) {
            if (texture != nullptr) {
                SDL_DestroyTexture(texture);
            }
        }
    };

    SDL_Renderer* renderer_;
    std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDelete> > texture_;
};
} // resource
// engine

#endif //TEXTURE_MANAGER_H
