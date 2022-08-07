package audio

import mixer "vendor:sdl2/mixer"
import fmt "core:fmt"
import sdl "vendor:sdl2"

AudioManager :: struct {
    sounds: [dynamic]^mixer.Chunk,
    music: [dynamic]^mixer.Music,
}

initAudioManager :: proc() {
    if (mixer.OpenAudio(44100, mixer.DEFAULT_FORMAT, 2, 2048) < 0) {
        fmt.println("Failed to initialize Mix. ", mixer.GetError())
    }
}

deinitAudioManager :: proc() {
    mixer.Quit()
}

is_music_playing :: proc() -> bool { return mixer.PlayingMusic() != 0 }

play_music :: proc(manager: ^AudioManager, key: u32, loop: i32) -> bool { 
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

play_sound :: proc(manager: ^AudioManager, key: i32) -> i32 {
    return mixer.PlayChannel(-1, manager.sounds[key], 0)
}

load_and_save_music :: proc(manager: ^AudioManager, path: cstring) -> (success: bool, id: int) {
    music := mixer.LoadMUS(path)
    
    if music == nil {
        sdl.Log("Unable to load music! SDL_mixer Error: %s\n", mixer.GetError())
        return false, -1
    }
    
    append(&manager.music, music)
    return true, len(manager.music) - 1
}

load_and_save_sound_effect :: proc(manager: ^AudioManager, path: cstring) -> int {
    chunk := mixer.LoadWAV(path)
    if chunk == nil {
        sdl.Log("Unable to load sound! SDL_mixer Error: %s\n", mixer.GetError())
        return -1
    }
    append(&manager.sounds, chunk)
    return len(manager.sounds) - 1
}
