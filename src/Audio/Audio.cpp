#include "Audio.hpp"
#include <iostream>
#include <SDL_mixer.h>

AudioManager::AudioManager() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Failed to initialize Mix. " << Mix_GetError() << std::endl;
    }
}

AudioManager::~AudioManager() {
    Mix_Quit();
}

bool AudioManager::is_music_playing() { return Mix_PlayingMusic(); }
bool AudioManager::play_music(Uint32 key, int loop) { return Mix_PlayMusic(this->music[key], loop); }
bool AudioManager::pause_music() { 
    Mix_PauseMusic();
    return this->is_music_playing(); 
}
bool AudioManager::stop_music() {
    Mix_HaltMusic();
    return !this->is_music_playing();
}

bool AudioManager::is_music_paused() {
    return Mix_PausedMusic();
}

bool AudioManager::resume_music() {
    Mix_ResumeMusic();
    return this->is_music_playing();
}

int AudioManager::play_sound(Uint32 key) {
    return Mix_PlayChannel(-1, this->sounds[key], 0);
}
bool AudioManager::stop_sound(int channel) { return false; }

Uint32 AudioManager::load_and_save_music(std::string path) {
    Mix_Music *music = Mix_LoadMUS(path.c_str());
    
    if (music == nullptr) {
        SDL_Log("Unable to load music! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }
    
    Uint32 index = 0;
    while (this->music[++index] != nullptr);
    this->music[index] = music;
    return index;
}

Uint32 AudioManager::load_and_save_sound_effect(std::string path) {
    Mix_Chunk *chunk = Mix_LoadWAV(path.c_str());
    if (chunk == nullptr) {
        SDL_Log("Unable to load sound! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }
    Uint32 index = 0;
    while (this->sounds[++index] != nullptr);
    this->sounds[index] = chunk;
    return index;
}
