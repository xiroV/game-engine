package audio

import mixer "vendor:sdl2/mixer"
import fmt "core:fmt"
import sdl "vendor:sdl2"

AudioManager :: struct {
    sounds: map[int]^mixer.Chunk,
    music: map[int]^mixer.Music,
}

initAudioManager :: proc() -> ^AudioManager {
    if (mixer.OpenAudio(44100, mixer.DEFAULT_FORMAT, 2, 2048) < 0) {
        fmt.println("Failed to initialize Mix. ", mixer.GetError())
    }
    return new(AudioManager)
}

deinitAudioManager :: proc(audio: ^AudioManager) {
    mixer.Quit()
}

is_music_playing :: proc() -> bool { return mixer.PlayingMusic() != 0 }

play_music :: proc(manager: ^AudioManager, key: int, loop: i32) -> bool { 
    return mixer.PlayMusic(manager.music[key], loop) == 0
}

pause_music :: proc() -> bool { 
    mixer.PauseMusic()
    return is_music_playing() 
}

stop_music :: proc() -> bool {
    mixer.HaltMusic()
    return is_music_playing()
}

is_music_paused :: proc() -> bool {
    return mixer.PausedMusic() == 1
}

resume_music :: proc() -> bool {
    mixer.ResumeMusic()
    return is_music_playing()
}

play_sound :: proc(manager: ^AudioManager, key: int) -> i32 {
    return mixer.PlayChannel(-1, manager.sounds[key], 0)
}

load_and_save_music :: proc(manager: ^AudioManager, path: cstring) -> (success: bool, key: int) {
    music := mixer.LoadMUS(path)
    
    if music == nil {
        sdl.Log("Unable to load music! SDL_mixer Error: %s\n", mixer.GetError())
        success = false
        key = -1
        return
    }
    
    key = len(manager.music)
    manager.music[key] = music
    success = true
    return
}

load_and_save_sound_effect :: proc(manager: ^AudioManager, path: cstring) -> (key: int) {
    chunk := mixer.LoadWAV(path)
    if chunk == nil {
        sdl.Log("Unable to load sound! SDL_mixer Error: %s\n", mixer.GetError())
        return -1
    }
    key = len(manager.sounds)
    manager.sounds[key] = chunk
    return
}
