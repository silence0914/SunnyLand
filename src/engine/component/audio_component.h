//
// Created by wangm on 26-10-29.
//

#ifndef AUDIO_COMPONENT_H
#define AUDIO_COMPONENT_H
#include <unordered_map>
#include "component.h"

namespace engine::component {
class TransformComponent;
}

namespace engine::render {
class Camera;
}

namespace engine::audio {
class AudioPlayer;
}

namespace engine {
namespace component {
class AudioComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    AudioComponent(engine::audio::AudioPlayer& audio_player, engine::render::Camera& camera);
    ~AudioComponent() override = default;
    AudioComponent(const AudioComponent& other) = delete;
    AudioComponent(AudioComponent&& other) noexcept = delete;
    AudioComponent& operator=(const AudioComponent& other) = delete;
    AudioComponent& operator=(AudioComponent&& other) noexcept = delete;

    /**
     * @brief 播放音效。
     * @param sound_id 音效文件的id (或路径)。
     * @param channel 要播放的特定通道，或 -1 表示第一个可用通道。
     * @param use_spatial 是否使用空间定位。
     */
    void playSound(const std::string& sound_id, int channel = -1, bool use_spatial = false);

    void addSound(const std::string& sound_id, const std::string& sound_path);

protected:
    void init() override;
    void update(const float, engine::core::Context&) override {};

private:
    engine::component::TransformComponent* transform_component_;
    engine::audio::AudioPlayer& audio_player_;
    engine::render::Camera& camera_;

    std::unordered_map<std::string, std::string> sound_id_to_path; // 音效  id与路径映射；
};
} // component
} // engine

#endif //AUDIO_COMPONENT_H
