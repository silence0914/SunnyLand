//
// Created by wangm on 26-10-11.
//

#include "font_manager.h"
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace engine {
namespace resource {
font_manager::font_manager() {
    if (!TTF_WasInit() && !TTF_Init()) {
        throw std::runtime_error("create FontManager error; TTF_Init failed; error :" + std::string(SDL_GetError()));
    }
    spdlog::trace("create FontManager success");
}

font_manager::~font_manager() {
    clearFonts();
    TTF_Quit();
    spdlog::trace("FontManager delete completion");
}

TTF_Font* font_manager::loadFont(const std::string& file_path, const int font_size) {
    FontKey key{file_path, font_size};
    auto it = font_.find(key);
    if (it != font_.end()) {
        spdlog::warn("load font exist; {} {}", file_path, font_size);
        return it->second.get();
    }
    spdlog::trace("load font; {} {}", file_path, font_size);
    TTF_Font* raw_font = TTF_OpenFont(file_path.c_str(), font_size);
    if (raw_font == nullptr) {
        spdlog::error("load font failed; {} , {} : {}", file_path, font_size, SDL_GetError());
        return nullptr;
    }
    font_.emplace(key, std::unique_ptr<TTF_Font, SDLFontDelete>(raw_font));
    spdlog::debug("load font success; {} {}", file_path, font_size);
    return raw_font;
}

TTF_Font* font_manager::getFont(const std::string& file_path, int font_size) {
    FontKey key{file_path, font_size};
    auto it = font_.find(key);
    if (it != font_.end()) {
        return it->second.get();
    }
    return loadFont(file_path, font_size);
}

void font_manager::unloadFont(const std::string& file_path, int font_size) {
    FontKey key{file_path, font_size};
    auto it = font_.find(key);
    if (it == font_.end()) {
        spdlog::warn("unload font nonexistence, {} {}", file_path, font_size);
        return;
    }
    font_.erase(it);
    spdlog::debug("unload font success; {} {}", file_path, font_size);
}

void font_manager::clearFonts() {
    spdlog::trace("clear font");
    if (!font_.empty()) {
        font_.clear();
    }
}
} // resource
} // engine
