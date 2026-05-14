#pragma once

#include <string>
#include "soloud.h"
#include "soloud_wavstream.h"

class AudioManager {
private:
    SoLoud::Soloud engine;
    SoLoud::WavStream backgroundMusic;
    SoLoud::handle backgroundHandle = 0;

    bool initialized = false;
    bool loopBackground = false;
    float volume = 1.0f;

public:
    AudioManager();
    ~AudioManager();

    bool PlayBackgroundMusic(const std::string& filePath);
    void StopBackgroundMusic();
    void PauseBackgroundMusic();
    void ResumeBackgroundMusic();
    void SetLoop(bool loop);
    void SetVolume(float newVolume);
};