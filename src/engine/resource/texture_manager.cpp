//
// Created by wangm on 26-10-11.
//

#include "texture_manager.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <SDL3_image/SDL_image.h>

namespace engine {
namespace resource {
TextureManager::TextureManager(SDL_Renderer* renderer) : renderer_(renderer) {
    if (renderer_ == nullptr) {
        throw std::runtime_error("create TextureManager failed, renderer nullptr");
    }
    spdlog::trace("create TextureManager success");
}

TextureManager::~TextureManager() {
    clearTextures();
    spdlog::trace("TextureManager delete completion");
}

SDL_Texture* TextureManager::loadTexture(const std::string& file_path) {
    auto it = texture_.find(file_path);
    if (it != texture_.end()) {
        return it->second.get();
    }
    spdlog::trace("load texture; {}", file_path);
    SDL_Texture* raw_texture = IMG_LoadTexture(renderer_, file_path.c_str());
    if (raw_texture == nullptr) {
        spdlog::error("load texture failed; {} : {}", file_path, SDL_GetError());
        return nullptr;
    }
    texture_.emplace(file_path, std::unique_ptr<SDL_Texture, SDLTextureDelete>(raw_texture));
    spdlog::debug("load texture success; {}", file_path);
    return raw_texture;
}

SDL_Texture* TextureManager::getTexture(const std::string& file_path) {
    auto it = texture_.find(file_path);
    if (it != texture_.end()) {
        return it->second.get();
    }
    return loadTexture(file_path);
}

void TextureManager::unloadTexture(const std::string& file_path) {
    auto it = texture_.find(file_path);
    if (it == texture_.end()) {
        spdlog::warn("unload texture nonexistence, {}", file_path);
        return;
    }
    texture_.erase(it);
    spdlog::debug("unload texture success; {}", file_path);
}

void TextureManager::clearTextures() {
    spdlog::trace("clear texture");
    if (!texture_.empty()) {
        texture_.clear();
    }
}
} // resource
} // engine
