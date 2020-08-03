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
		rendering.draw_stored_texture(schulz_texture_key, nullptr, nullptr);
		rendering.render_text("Hello", WHITE, 24, SDL_Point{(1280 / 2), 720 / 2});
		rendering.render();
	}
}
