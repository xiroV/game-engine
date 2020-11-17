#include <SDL_mixer.h>
#include <SDL2/SDL.h>
#include <map>
#include <string>

class AudioManager {
	private:
		std::map<Uint32, Mix_Chunk*> sounds;
		std::map<Uint32, Mix_Music*> music;
	public:
		AudioManager();
		~AudioManager();
		bool is_music_playing();
		bool play_music(Uint32 key, int loops);
		bool pause_music();
		bool is_music_paused();
		bool resume_music();
		bool stop_music();
		int play_sound(Uint32 key);
		bool stop_sound(int channel);
		Uint32 load_and_save_music(std::string path);
		Uint32 load_and_save_sound_effect(std::string path);
};