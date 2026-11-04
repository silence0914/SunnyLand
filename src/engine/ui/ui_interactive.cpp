//
// Created by wangm on 26-11-3.
//

#include "ui_interactive.h"

#include "state/ui_state.h"
#include "../audio/audio_player.h"
#include "../core/context.h"
#include "../log/my_log.h"
#include "../resource/resource_manager.h"

namespace engine {
namespace ui {
UiInteractive::UiInteractive(engine::core::Context& context, const glm::vec2& position,
                             const glm::vec2& size): UiElement(position, size), context_(context) {}

void UiInteractive::addSprite(const std::string& name, std::unique_ptr<engine::render::Sprite> sprite) {
    if (name.empty() || !sprite) {
        mylog::MyLog{}.warn("addSprite failed; name.empty() || !sprite ");
        return;
    }
    auto it = sprites_.find(name);
    if (it != sprites_.end()) {
        mylog::MyLog{}.warn("addSprite failed; key '{}' is already exist\n old sprite : '{}' \n new sprite : '{}'",
                            name, it->second->getTextureId(), sprite->getTextureId());
    }
    if (size_.x == 0.0f || size_.y == 0.0f) {
        size_ = context_.getResourceManager().getTextureSize(sprite->getTextureId());
    }
    sprites_[name] = std::move(sprite);
}

void UiInteractive::setSprite(std::string_view name) {
    if (name.empty()) {
        mylog::MyLog{}.warn("setSprite failed; name empty");
        return;
    }
    auto it = sprites_.find(std::string{name});
    if (it == sprites_.end()) {
        mylog::MyLog{}.warn("setSprite failed; key '{}' is not exist", name);
        return;
    }
    current_sprite_ = it->second.get();
}


void UiInteractive::addSound(const std::string& name, const std::string& path) {
    if (name.empty() || path.empty()) {
        mylog::MyLog{}.warn("addSound failed; name.empty() || path.empty()");
        return;
    }
    auto it = sounds_.find(name);
    if (it != sounds_.end()) {
        mylog::MyLog{}.warn("addSound failed; key '{}' is already exist;\n old path: '{}' \n new path: '{}' ",
                            name, it->second, path);
        return;
    }
    sounds_[name] = path;
}

void UiInteractive::playSound(const std::string& name) {
    auto it = sounds_.find(name);
    if (it == sounds_.end()) {
        mylog::MyLog{}.warn("playSound failed; key '{}' is not exist", name);
        return;
    }
    context_.getAudioPlayer().playSound(it->second);
}

bool UiInteractive::handleInput(engine::core::Context& context) {
    if (UiElement::handleInput(context)) { return true; }
    if (state_ && interactive_) {
        if (auto next_state = state_->handleInput(context)) {
            setState(std::move(next_state));
            return true;
        }
    }
    return false;
}

void UiInteractive::render(engine::core::Context& context) {
    if (!visible_) { return; }
    context.getRenderer().drawUISprite(*current_sprite_, getScreenPosition(), size_);

    UiElement::render(context);
}

void UiInteractive::setState(std::unique_ptr<engine::ui::state::UiState> state) {
    if (!state) {
        mylog::MyLog{}.warn("setState failed; state is null");
        return;
    }
    state_ = std::move(state);
    state_->enter();
}
} // ui
} // engine
