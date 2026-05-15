#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() {
    SoLoud::result result = engine.init();

    if (result != SoLoud::SO_NO_ERROR) {
        std::cout << "Failed to initialize SoLoud audio engine." << std::endl;
        initialized = false;
    }
    else {
        initialized = true;
    }
}

AudioManager::~AudioManager() {
    StopBackgroundMusic();

    if (initialized) {
        engine.deinit();
    }
}

bool AudioManager::PlayBackgroundMusic(const std::string& filePath) {
    if (!initialized) {
        std::cout << "Audio engine is not initialized." << std::endl;
        return false;
    }

    StopBackgroundMusic();

    SoLoud::result result = backgroundMusic.load(filePath.c_str());

    if (result != SoLoud::SO_NO_ERROR) {
        std::cout << "Failed to load background music: " << filePath << std::endl;
        return false;
    }

    backgroundMusic.setLooping(loopBackground);

    backgroundHandle = engine.play(backgroundMusic, volume);

    if (!engine.isValidVoiceHandle(backgroundHandle)) {
        std::cout << "Failed to play background music." << std::endl;
        return false;
    }

    return true;
}

bool AudioManager::PlaySoundEffect(const std::string& filePath, float effectVolume) {
    if (!initialized) return false;

    StopSoundEffect();

    SoLoud::result result = soundEffect.load(filePath.c_str());
    if (result != SoLoud::SO_NO_ERROR) return false;

    soundEffect.setLooping(false);
    soundEffectHandle = engine.play(soundEffect, effectVolume);

    return engine.isValidVoiceHandle(soundEffectHandle);
}

void AudioManager::StopBackgroundMusic() {
    if (backgroundHandle != 0 && engine.isValidVoiceHandle(backgroundHandle)) {
        engine.stop(backgroundHandle);
    }

    backgroundHandle = 0;
}
void AudioManager::StopSoundEffect() {
    if (soundEffectHandle != 0 && engine.isValidVoiceHandle(soundEffectHandle)) {
        engine.stop(soundEffectHandle);
    }

    soundEffectHandle = 0;
}

void AudioManager::PauseBackgroundMusic() {
    if (backgroundHandle != 0 && engine.isValidVoiceHandle(backgroundHandle)) {
        engine.setPause(backgroundHandle, true);
    }
}

void AudioManager::ResumeBackgroundMusic() {
    if (backgroundHandle != 0 && engine.isValidVoiceHandle(backgroundHandle)) {
        engine.setPause(backgroundHandle, false);
    }
}

void AudioManager::SetLoop(bool loop) {
    loopBackground = loop;
    backgroundMusic.setLooping(loop);

    if (backgroundHandle != 0 && engine.isValidVoiceHandle(backgroundHandle)) {
        engine.setLooping(backgroundHandle, loop);
    }
}
