//
// Created by wangm on 26-10-11.
//

#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H
#include <string>
#include <memory>
#include <unordered_map>
#include <SDL3_ttf/SDL_ttf.h>

namespace engine {
namespace resource {
using FontKey = std::pair<std::string, int>;

struct FontKeyHash {
    std::size_t operator()(const FontKey& font_key) const {
        std::hash<std::string> string_hash;
        std::hash<int> int_hash;
        return string_hash(font_key.first) ^ int_hash(font_key.second);
    }
};

class font_manager {
    friend class ResourceManager;

public:
    font_manager();
    ~font_manager();

    font_manager(const font_manager& other) = delete;
    font_manager(font_manager&& other) = delete;
    font_manager& operator=(const font_manager& other) = delete;
    font_manager& operator=(font_manager&& other) = delete;

private:
    TTF_Font* loadFont(const std::string& file_path, int font_size); // 加载字体
    TTF_Font* getFont(const std::string& file_path, int font_size); // 获取字体
    void unloadFont(const std::string& file_path, int font_size); // 卸载字体
    void clearFonts(); // 清空字体

private:
    struct SDLFontDelete {
        void operator()(TTF_Font* font) {
            if (font != nullptr) {
                TTF_CloseFont(font);
            }
        }
    };

    std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDelete>, FontKeyHash> font_;
};
} // resource
} // engine

#endif //FONT_MANAGER_H
