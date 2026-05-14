#include "AudioManager.h"

AudioManager::AudioManager() {}

AudioManager::~AudioManager() {
    StopBackgroundMusic();
}

void AudioManager::PlayBackgroundMusic(const std::string& filePath) {
    if (!backgroundMusic.openFromFile(filePath)) {
        return;
    }
    backgroundMusic.play();
}

void AudioManager::StopBackgroundMusic() {
    backgroundMusic.stop();
}

void AudioManager::PauseBackgroundMusic() {
    backgroundMusic.pause();
}

void AudioManager::ResumeBackgroundMusic() {
    backgroundMusic.play();
}

void AudioManager::SetLoop(bool loop) {
    backgroundMusic.setLooping(loop);
}