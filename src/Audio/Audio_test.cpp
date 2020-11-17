#include "Audio.hpp"
#include "../Rendering/Rendering.hpp"
#include "../Engine.h"
#include <iostream>

void audio_test() {
	AudioManager audio;
	Rendering rendering;
	Input i;
	rendering.load_default_font("./assets/Font/PressStart2P.ttf", 32);
	auto boing_audio_id = audio.load_and_save_sound_effect("./assets/Sounds/boing.wav");
	auto elevator_music_audio_id = audio.load_and_save_music("./assets/Sounds/beat.wav");
	std::cout << boing_audio_id << std::endl;
	std::cout << elevator_music_audio_id << std::endl;
	while (!i.handle_input()) {
		
		rendering.draw_text("Press X to boing", WHITE, 25, 25, 25);
		rendering.draw_text("Press D to play smooth muzak.", WHITE, 25, 25, 55);
		rendering.show();

		if (i.is_key_pressed(SDLK_x)) {
			std::cout << audio.play_sound(boing_audio_id) << std::endl;
		}

		if (i.is_key_pressed(SDLK_d)) {
			if (audio.is_music_playing()) {
				if (audio.is_music_paused()) audio.resume_music();
				else audio.pause_music();
			} else {
				audio.play_music(elevator_music_audio_id, -1);
			}
		}
	}
}