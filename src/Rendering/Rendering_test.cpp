#include "Rendering.hpp"
#include "../Input/Input.hpp"
#include <map>
#include <math.h>

int max(int a, int b) {
	if (a > b) return a;
	return b;
}

int min(int a, int b) {
	if (a < b) return a;
	return b;
}

void rendering_test() {
	Rendering rendering;
	Input input;

	rendering.load_default_font("./assets/Font/PressStart2P.ttf", 32);
	int farmer_key = rendering.load_and_save_texture("./assets/Images/farmer.png");
	int schulz_texture_key = rendering.load_and_save_texture("./assets/Images/schulz.png");
	SDL_Texture *farmer = rendering.get_texture(farmer_key);

	AnimationReference anim_ref(farmer, 39, 64, 4, true, true);
	Animation anim(&anim_ref, 200);

	double angle = 0;
	bool ascending = false;
	Uint32 now = 0;
	Uint32 last_new = 0;
	double delta = 0;
	int x = 0;
	int y = 0;

	while (!input.handle_input()) {

		SDL_GetMouseState(&x, &y);
		//Render texture to screen
		now = SDL_GetTicks();
		delta = now - last_new;
		last_new = now;

		double a = abs(x - 300);
		double b = abs(y - 300);
		double c = sqrt(a * a + b * b);

		SDL_Rect r = SDL_Rect{ 200, 200, 200, 200 };
		rendering.draw_text("No touching", WHITE, 12, r.x + 20, r.y + 100);
		rendering.set_texture_alpha(schulz_texture_key, c < 141 ? 0 : 0 + max(0, min(255, c)));
		rendering.draw_stored_texture(schulz_texture_key, &r);
		rendering.set_texture_alpha(schulz_texture_key, 255);
		SDL_Rect r2 = SDL_Rect{ 800, 200, 200, 200 };

		rendering.draw_stored_texture_rotated(schulz_texture_key, &r2, nullptr, angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
		rendering.draw_text("Hello", WHITE, 24, 1280 / 2, 720 / 2);
		rendering.draw_line(1280 / 2, 0, 1280 / 2, 720, WHITE);
		rendering.draw_line(0, 720 / 2, 1280, 720 / 2, WHITE);
		rendering.draw_rect(1280 / 2 - 60, 720 / 2 - 60, 120, 120, WHITE, false);
		rendering.draw_animation(&anim, 400, 100);
		rendering.show();

		if (ascending) {
			angle += (delta / 20);
		} else {
			angle -= (delta / 20);
		}

		if (abs(angle) > 30) ascending = !ascending;

		anim.update(delta);
	}
}