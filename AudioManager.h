#pragma once

#include <string>
#include "soloud.h"
#include "soloud_wavstream.h"
#include "soloud_wav.h"


class AudioManager {
private:
    SoLoud::Soloud engine;
    SoLoud::WavStream backgroundMusic;
    SoLoud::Wav soundEffect;

    SoLoud::handle backgroundHandle = 0;
    SoLoud::handle soundEffectHandle = 0;


    bool initialized = false;
    bool loopBackground = false;
    float volume = 1.0f;

public:
    AudioManager();
    ~AudioManager();

    bool PlayBackgroundMusic(const std::string& filePath);
    bool PlaySoundEffect(const std::string& filePath, float effectVolume = 1.0f);

    void StopBackgroundMusic();
    void StopSoundEffect();

    void PauseBackgroundMusic();
    void ResumeBackgroundMusic();
    void SetLoop(bool loop);
    void SetVolume(float newVolume);
};