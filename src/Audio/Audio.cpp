#include "Audio.hpp"
#include <iostream>

AudioManager::AudioManager() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Failed to initialize Mix." << std::endl;
    }
}
bool AudioManager::is_music_stopped() { return false; }
bool AudioManager::play_music(Uint32 key) { return false; }
bool AudioManager::stop_music() { return false; }
bool AudioManager::play_sound(Uint32 key) { return false; }
bool AudioManager::stop_sound(Uint32 key) { return false; }

Uint32 AudioManager::load_and_save_music(std::string path) {
    Mix_Music *music = Mix_LoadMUS(path.c_str());
    
    if (music == nullptr) {
        SDL_Log("Unable to load music! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }
    
    Uint32 index = 0;
    while (this->music[index++] != nullptr);
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
    while (this->sounds[index++] != nullptr);
    this->sounds[index] = chunk;
    return index;
}
