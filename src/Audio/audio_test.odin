package audio

import fmt "core:fmt"
import ren "../rendering"
import audio "./audio"

audio_test :: proc() {
	audio: AudioManager
	rendering: Rendering
	i: Input
	load_default_font(rendering, "./assets/Font/PressStart2P.ttf", 32)
	boing_audio_id := load_and_save_sound_effect(audio, "./assets/Sounds/boing.wav")
	elevator_music_audio_id := load_and_save_music(audio, "./assets/Sounds/beat.wav")
	fmt.println(boing_audio_id)
	fmt.println(elevator_music_audio_id)
	for !i.handle_input() {
		
		draw_text(rendering, "Press X to boing", WHITE, 25, 25, 25);
		draw_text(rendering, "Press D to play smooth muzak.", WHITE, 25, 25, 55);
		show(rendering);

		if i.is_key_pressed(SDLK_x) {
			audio.play_sound(boing_audio_id) << std::endl;
		}

		if i.is_key_pressed(SDLK_d) {
			if (audio.is_music_playing()) {
				if is_music_paused(audio) do resume_music(audio)
				else pause_music(audio)
			} else {
				play_music(audio, elevator_music_audio_id, -1);
			}
		}
	}
}