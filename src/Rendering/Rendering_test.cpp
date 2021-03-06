#include "Rendering.hpp"
#include "../Input/Input.hpp"
#include <map>
#include <iostream>
#include <cmath>

int max(int a, int b) {
	return a > b ? a : b;
}

int min(int a, int b) {
	return a < b ? a : b;
}

void rendering_test() {
	Rendering rendering;
	Input input;

	rendering.load_default_font("./assets/Font/PressStart2P.ttf", 32);
	int farmer_key = rendering.load_and_save_texture("./assets/Images/farmer.png");
	int schulz_texture_key = rendering.load_and_save_texture("./assets/Images/schulz.png");
    int windmill_base_key = rendering.load_and_save_texture("./assets/Images/windmill_base.png");
    int windmill_blades_key = rendering.load_and_save_texture("./assets/Images/windmill_blades.png");
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
    SDL_Point windmill_center{180 / 2, 186 / 2};           
    SDL_Rect windmill_base{500, 100, 224 / 2, 463 / 2};
    SDL_Rect windmill_blades{514, 22, 338 / 2, 380 / 2};
    SDL_Rect r = SDL_Rect{ 200, 200, 200, 200 };
    SDL_Rect r2 = SDL_Rect{ 800, 200, 200, 200 };
    double windmill_rotation = 0.f;
	std::string no_touching_text = "No touching";
	Uint8 no_touching_size = no_touching_text.length();
	std::string hello_text = "Hello";
	Uint8 hello_size = hello_text.length();
	int no_touching_key = rendering.store_text_as_texture(no_touching_text, nullptr, WHITE);
	int hello_key = rendering.store_text_as_texture(hello_text, nullptr, WHITE);

	while (!input.handle_input()) {
		x = input.mouse_position.x;
		y = input.mouse_position.y;
		now = SDL_GetTicks();
		delta = now - last_new;
		last_new = now;

		double a = abs(x - 300);
		double b = abs(y - 300);
		double c = hypot(a, b);

		rendering.draw_stored_text(no_touching_key, r.x + 20, r.y + 100, 12, no_touching_size);
		rendering.set_texture_alpha(schulz_texture_key, c < 141 ? 0 : 0 + max(0, min(255, c)));
		rendering.draw_stored_texture(schulz_texture_key, &r);
		rendering.set_texture_alpha(schulz_texture_key, 255);

		rendering.draw_stored_texture_rotated(schulz_texture_key, &r2, nullptr, angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
		rendering.draw_stored_text(hello_key, 1280 / 2, 720 / 2, 24, hello_size);
		rendering.draw_line(1280 / 2, 0, 1280 / 2, 720, WHITE);
		rendering.draw_line(0, 720 / 2, 1280, 720 / 2, WHITE);
		rendering.draw_rect(1280 / 2 - 60, 720 / 2 - 60, 120, 120, WHITE, false);
		rendering.draw_animation(&anim, 400, 100);
		rendering.draw_animation_rotated(&anim, 459, 100, angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);

        rendering.draw_stored_texture(windmill_base_key, &windmill_base, nullptr);
        rendering.draw_stored_texture_rotated(windmill_blades_key, &windmill_blades, nullptr, windmill_rotation, &windmill_center, SDL_RendererFlip::SDL_FLIP_NONE);
		rendering.show();

		if (ascending) {
			angle += (delta / 20);
			if (angle > 30) ascending = !ascending;
		} else {
			angle -= (delta / 20);
			if (angle < -30) ascending = !ascending;
		}

        windmill_rotation += (delta / 10);
        if (windmill_rotation > 360.f) {
            windmill_rotation -= 360;
        }

		anim.update(delta);
	}
}