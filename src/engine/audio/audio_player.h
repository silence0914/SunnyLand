//
// Created by wangm on 26-10-29.
//

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H
#include <string>

namespace engine::resource {
class ResourceManager;
}

namespace engine {
namespace audio {
class AudioPlayer final {
public:
    explicit AudioPlayer(engine::resource::ResourceManager& resource_manager);
    ~AudioPlayer() = default;

    AudioPlayer(const AudioPlayer& other) = delete;
    AudioPlayer(AudioPlayer&& other) noexcept = delete;
    AudioPlayer& operator=(const AudioPlayer& other) = delete;
    AudioPlayer& operator=(AudioPlayer&& other) noexcept = delete;

    /**
     * @brief 播放音效
     * @param sound_path 音效文件路径
     * @param channel 要播放音效的通道  默认-1  表示第一个可用的通道
     * @return 音效正在播放的通道， 出错时返回-1
     */
    int playSound(const std::string& sound_path, int channel = -1);

    /**
     * @brief 播放背景音乐  如果正在播放  则淡出之前的音乐
     * @param music_path  音乐文件路径
     * @param loops 循环次数 -1无限循环 0播放一次
     * @param fade_in_ms 音乐淡入的时间（ms)
     * @return
     */
    bool playMusic(const std::string& music_path, int loops = -1, int fade_in_ms = 0);

    /**
     * @brief 停止当前播放的背景音乐
     * @param fade_out_ms 淡出时间  默认为0；
     */
    void stopMusic(int fade_out_ms = 0);

    /**
     * @brief 暂停当前背景音乐
     */
    void pauseMusic();

    /**
     * @brief 恢复暂停的背景音乐
     */
    void resumeMusic();

    /**
     * @brief 设置音效通道音量
     * @param volume 音量级别 0.0 - 1.0
     * @param channel 通道号；-1代表所有 默认-1
     */
    void setSoundVolume(float volume, int channel = -1);

    /**
     * @brief 设置音乐音量；
     * @param volume 音量大小
     */
    void setMusicVolume(float volume);

    /**
     * @brief 获取当前音乐音量
     * @return 音量大小 0.0 - 1.0;
     */
    float getMusicVolume();

    /**
     * @brief 获取对应通道的音效音量；
     * @param channel 音效通道；  -1表示所有通道；  默认-1
     * @return 音量大小 0.0 - 1.0;
     */
    float getSoundVolume(int channel = -1);

private:
    engine::resource::ResourceManager& resource_manager_;
    std::string current_music_; // 当前正在播放的音乐路径；
};
} // audio
} // engine

#endif //AUDIO_PLAYER_H
