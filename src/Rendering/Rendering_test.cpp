#include "Rendering.hpp"
#include "../Input/Input.hpp"
#include <map>

void rendering_test() {
	Rendering rendering;
	Input input;

	rendering.load_default_font("./assets/Font/PressStart2P.ttf", 32);
	int schulz_texture_key = rendering.load_and_save_texture("./assets/Images/schulz.png");

	while (!input.handle_input()) {
		//Render texture to screen
		SDL_Rect r = SDL_Rect{ 200, 200, 200, 200 };
		rendering.draw_stored_texture(schulz_texture_key, &r, nullptr);
		rendering.draw_text("Hello", WHITE, 24, 1280 / 2, 720 / 2);
		rendering.draw_line(1280 / 2, 0, 1280 / 2, 720, WHITE);
		rendering.draw_line(0, 720 / 2, 1280, 720 / 2, WHITE);
		rendering.draw_rect(1280 / 2 - 60, 720 / 2 - 60, 120, 120, WHITE, false);
		rendering.show();
	}
}
