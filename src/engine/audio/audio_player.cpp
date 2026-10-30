//
// Created by wangm on 26-10-29.
//

#include "audio_player.h"

#include "../log/log_manager.h"
#include "../log/my_log.h"
#include "../resource/resource_manager.h"
#include "SDL3_mixer/SDL_mixer.h"

namespace engine {
namespace audio {
AudioPlayer::AudioPlayer(engine::resource::ResourceManager& resource_manager): resource_manager_(resource_manager) {
    mylog::MyLog{}.info("create AudioPlayer success");
}

int AudioPlayer::playSound(const std::string& sound_path, int channel) {
    Mix_Chunk* chunk = resource_manager_.getSound(sound_path);
    if (!chunk) {
        mylog::MyLog{}.error("getSound false, sound_path = '{}'", sound_path);
        return -1;
    }
    int played_channel = Mix_PlayChannel(channel, chunk, 0);
    if (played_channel == -1) {
        mylog::MyLog{}.error("playSound false, '{}' : '{}'", sound_path, SDL_GetError());
        return -1;
    } else {
        mylog::MyLog{}.trace("playSound success, sound_path = '{}'", sound_path);
    }
    return played_channel;
}

bool AudioPlayer::playMusic(const std::string& music_path, int loops, int fade_in_ms) {
    if (music_path == current_music_) { return true; }
    current_music_ = music_path;
    Mix_Music* music = resource_manager_.getMusic(music_path);
    if (!music) {
        mylog::MyLog{}.error("getMusic false, music_path = '{}'", music_path);
        return false;
    }
    Mix_HaltMusic();
    bool result = false;
    if (fade_in_ms > 0) {
        result = Mix_FadeInMusic(music, loops, fade_in_ms); // 淡入播放音乐
    } else {
        result = Mix_PlayMusic(music, loops);
    }

    if (!result) {
        mylog::MyLog{}.error("playMusic false, '{}' : '{}'", music_path, SDL_GetError());
    } else {
        mylog::MyLog{}.trace("playMusic success, music_path = '{}'", music_path);
    }
    return result;
}

void AudioPlayer::stopMusic(int fade_out_ms) {
    if (fade_out_ms > 0) {
        Mix_FadeOutMusic(fade_out_ms);
    } else {
        Mix_HaltMusic();
    }
    mylog::MyLog{}.trace("stopMusic");
}

void AudioPlayer::pauseMusic() {
    Mix_PauseMusic();
    mylog::MyLog{}.trace("pauseMusic");
}

void AudioPlayer::resumeMusic() {
    Mix_ResumeMusic();
    mylog::MyLog{}.trace("resumeMusic");
}

void AudioPlayer::setSoundVolume(float volume, int channel) {
    // 将浮点数转换成SDL_mixer的音量（0-128）
    int sdl_volume = static_cast<int>(std::max(0.0f, std::min(1.0f, volume) * MIX_MAX_VOLUME));
    Mix_Volume(channel, sdl_volume);
    mylog::MyLog{}.trace("setSoundVolume, volume = {}, channel = {}", volume, channel);
}

void AudioPlayer::setMusicVolume(float volume) {
    // 将浮点数转换成SDL_mixer的音量（0-128）
    int sdl_volume = static_cast<int>(std::max(0.0f, std::min(1.0f, volume) * MIX_MAX_VOLUME));
    Mix_VolumeMusic(sdl_volume);
    mylog::MyLog{}.trace("setMusicVolume, volume = {}", volume);
}

float AudioPlayer::getMusicVolume() {
    return static_cast<float>(Mix_VolumeMusic(-1)) / static_cast<float>(MIX_MAX_VOLUME);
}

float AudioPlayer::getSoundVolume(int channel) {
    return static_cast<float>(Mix_Volume(channel, -1)) / static_cast<float>(MIX_MAX_VOLUME);
}
} // audio
} // engine
