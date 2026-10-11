//
// Created by wangm on 26-10-9.
//

#include "audio_manager.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine {
namespace resource {
AudioManager::AudioManager() {
    MIX_InitFlags flags = MIX_INIT_OGG | MIX_INIT_MP3;
    if ((Mix_Init(flags) & flags) != flags) {
        throw std::runtime_error("AudioManager error: Mix_init failed : " + std::string(SDL_GetError()));
    }

    if (!Mix_OpenAudio(0, nullptr)) {
        Mix_Quit();
        throw std::runtime_error("AudioManager error: Mix_OpenAudio failed : " + std::string(SDL_GetError()));
    }
    spdlog::trace("create AudioManager success");
}

AudioManager::~AudioManager() {
    Mix_HaltChannel(-1); // 停止所有音效；
    Mix_HaltMusic(); // 停止音乐；

    clearSound();
    clearMusic();

    Mix_CloseAudio();
    Mix_Quit();
    spdlog::trace("delete audioManager completion");
}

Mix_Chunk* AudioManager::loadSound(const std::string& file_path) {
    auto it = sounds_.find(file_path);
    if (it != sounds_.end()) {
        return it->second.get();
    }
    spdlog::trace("load sound; {}", file_path);
    Mix_Chunk* raw_chunk = Mix_LoadWAV(file_path.c_str());
    if (raw_chunk == nullptr) {
        spdlog::error("load sound failed; {} : {};", file_path, SDL_GetError());
        return nullptr;
    }
    sounds_.emplace(file_path, std::unique_ptr<Mix_Chunk, SDLMixChunkDelete>(raw_chunk));
    spdlog::debug("load sound succeed; {}", file_path);
    return raw_chunk;
}

Mix_Chunk* AudioManager::getSound(const std::string& file_path) {
    auto it = sounds_.find(file_path);
    if (it != sounds_.end()) {
        return it->second.get();
    }
    return loadSound(file_path);
}

void AudioManager::unloadSound(const std::string& file_path) {
    auto it = sounds_.find(file_path);
    if (it != sounds_.end()) {
        sounds_.erase(it);
        spdlog::debug("unload sound succeed; {}", file_path);
    } else {
        spdlog::warn("unload sound nonexistence； {}", file_path);
    }
}

void AudioManager::clearSound() {
    spdlog::trace("clear sound");
    if (!sounds_.empty()) {
        sounds_.clear();
    }
}

Mix_Music* AudioManager::loadMusic(const std::string& file_path) {
    auto it = music_.find(file_path);
    if (it != music_.end()) {
        return it->second.get();
    }
    spdlog::trace("load music; {}", file_path);
    Mix_Music* raw_music = Mix_LoadMUS(file_path.c_str());
    if (raw_music == nullptr) {
        spdlog::error("load music failed; {}", file_path);
        return nullptr;
    }
    music_.emplace(file_path, std::unique_ptr<Mix_Music, SDLMixMusicDelete>(raw_music));
    spdlog::debug("loadMusic success; {}", file_path);
    return raw_music;
}

Mix_Music* AudioManager::getMusic(const std::string& file_path) {
    auto it = music_.find(file_path);
    if (it != music_.end()) {
        return it->second.get();
    }
    return loadMusic(file_path);
}

void AudioManager::unloadMusic(const std::string& file_path) {
    auto it = music_.find(file_path);
    if (it != music_.end()) {
        spdlog::debug("unload music nonexistence; {}", file_path);
        music_.erase(it);
    } else {
        spdlog::warn("unload music failed; {}", file_path);
    }
}

void AudioManager::clearMusic() {
    spdlog::trace("clear music");
    if (!music_.empty()) {
        music_.clear();
    }
}
} // resource
} // engine
