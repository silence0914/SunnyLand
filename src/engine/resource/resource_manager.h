//
// Created by wangm on 26-10-11.
//

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <memory>
#include <string>
struct TTF_Font;
struct SDL_Texture;
struct Mix_Chunk;
struct Mix_Music;
struct SDL_Renderer;

namespace engine::resource {
class AudioManager;
class font_manager;
class TextureManager;

class ResourceManager final {
public:
    explicit ResourceManager(SDL_Renderer* renderer);
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager& other) = delete;
    ResourceManager(ResourceManager&& other) noexcept = delete;
    ResourceManager& operator=(const ResourceManager& other) = delete;
    ResourceManager& operator=(ResourceManager&& other) noexcept = delete;

    TTF_Font* loadFont(const std::string& file_path, int font_size); // 加载字体
    TTF_Font* getFont(const std::string& file_path, int font_size); // 获取字体
    void unloadFont(const std::string& file_path, int font_size); // 卸载字体
    void clearFonts(); // 清空字体

    SDL_Texture* loadTexture(const std::string& file_path); // 加载纹理
    SDL_Texture* getTexture(const std::string& file_path); // 获取纹理
    void unloadTexture(const std::string& file_path); // 卸载纹理
    void clearTextures(); //清空纹理


    Mix_Chunk* loadSound(const std::string& file_path); // 加载音效
    Mix_Chunk* getSound(const std::string& file_path); // 获取音效
    void unloadSound(const std::string& file_path); // 卸载音效
    void clearSound(); // 清空音效

    Mix_Music* loadMusic(const std::string& file_path); // 加载音乐
    Mix_Music* getMusic(const std::string& file_path); // 获取音乐
    void unloadMusic(const std::string& file_path); // 卸载指定音乐
    void clearMusic(); // 清空音乐；

    void clear();
private:
    std::unique_ptr<AudioManager> audio_manager_;
    std::unique_ptr<font_manager> font_manager_;
    std::unique_ptr<TextureManager> texture_manager_;
};
} // resource engine

#endif //RESOURCE_MANAGER_H
