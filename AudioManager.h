#pragma once
#include <SFML/Audio.hpp>
#include <string>

class AudioManager {
private:
    sf::Music backgroundMusic;
public:
    AudioManager();
    ~AudioManager();
    
    void PlayBackgroundMusic(const std::string& filePath);
    void StopBackgroundMusic();
    void PauseBackgroundMusic();
    void ResumeBackgroundMusic();
    void SetLoop(bool loop);
};