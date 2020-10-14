#include <SDL_mixer.h>
#include <SDL2/SDL.h>
#include <map>
#include <string>

class AudioManager {
	std::map<Uint32, Mix_Chunk*> sounds;
	std::map<Uint32, Mix_Music*> music;

	AudioManager();
	bool is_music_stopped();
	bool play_music(Uint32 key);
	bool stop_music();
	bool play_sound(Uint32 key);
	bool stop_sound(Uint32 key);
	Uint32 load_and_save_music(std::string path);
	Uint32 load_and_save_sound_effect(std::string path);
};