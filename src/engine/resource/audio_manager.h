//
// Created by wangm on 26-10-9.
//

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <memory>
#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <unordered_map>

namespace engine::resource {
class AudioManager final {
    friend class ResourceManager;

public:
    AudioManager();
    ~AudioManager();

    AudioManager(const AudioManager& other) = delete;
    AudioManager(AudioManager&& other) = delete;
    AudioManager& operator=(const AudioManager& other) = delete;
    AudioManager& operator=(AudioManager&& other) = delete;

private:
    struct SDLMixChunkDelete {
        void operator()(Mix_Chunk* chunk) {
            if (chunk != nullptr) {
                Mix_FreeChunk(chunk);
            }
        }
    };

    struct SDLMixMusicDelete {
        void operator()(Mix_Music* music) {
            if (music != nullptr) {
                Mix_FreeMusic(music);
            }
        }
    };

    std::unordered_map<std::string, std::unique_ptr<Mix_Chunk, SDLMixChunkDelete> > sounds_;
    std::unordered_map<std::string, std::unique_ptr<Mix_Music, SDLMixMusicDelete> > music_;

private:
    Mix_Chunk* loadSound(const std::string& file_path); // 加载音效
    Mix_Chunk* getSound(const std::string& file_path); // 获取音效
    void unloadSound(const std::string& file_path); // 卸载音效
    void clearSound(); // 清空音效

    Mix_Music* loadMusic(const std::string& file_path); // 加载音乐
    Mix_Music* getMusic(const std::string& file_path); // 获取音乐
    void unloadMusic(const std::string& file_path); // 卸载指定音乐
    void clearMusic(); // 清空音乐；
};
} // resource
// engien

#endif //AUDIO_MANAGER_H
