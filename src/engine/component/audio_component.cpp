//
// Created by wangm on 26-10-29.
//

#include "audio_component.h"

#include "../log/log_manager.h"
#include "../log/my_log.h"
#include "../object/game_object.h"
#include "transform_component.h"
#include "../audio/audio_player.h"
#include "../render/camera.h"

namespace engine {
namespace component {
AudioComponent::AudioComponent(engine::audio::AudioPlayer& audio_player, engine::render::Camera& camera)
    : audio_player_(audio_player), camera_(camera) {
    mylog::MyLog{}.trace("create AudioComponent success");
}

void AudioComponent::playSound(const std::string& sound_id, int channel, bool use_spatial) {
    auto sound_path = sound_id_to_path.find(sound_id) != sound_id_to_path.end() ? sound_id_to_path[sound_id] : sound_id;

    if (use_spatial && transform_component_) {
        // 超过150像素范围不播放；
        auto camera_center = camera_.getPosition() + camera_.getViewportSize() / 2.0f;
        auto object_pos = transform_component_->getPosition();
        float distance = glm::length(camera_center - object_pos);
        if (distance > 150.0f) { return; }
        audio_player_.playSound(sound_path, channel);
    } else {
        audio_player_.playSound(sound_path, channel);
    }
}

void AudioComponent::addSound(const std::string& sound_id, const std::string& sound_path) {
    if (sound_id_to_path.find(sound_id) != sound_id_to_path.end()) {
        return;
    }
    sound_id_to_path[sound_id] = sound_path;
}

void AudioComponent::init() {
    if (!owner_) {
        mylog::MyLog{}.error("AudioComponent::init() owner_ is nullptr");
        return;
    }
    transform_component_ = owner_->getComponent<engine::component::TransformComponent>();
    if (!transform_component_) {
        mylog::MyLog{}.error("AudioComponent::init() transform_component_ is nullptr");
    }
}
} // component
} // engine
