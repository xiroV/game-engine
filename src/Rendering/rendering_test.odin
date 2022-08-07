package rendering

import rend "rendering"

rendering_test :: proc() {
	rendering: Rendering
	input: Input

	rendering.load_default_font("./assets/Font/PressStart2P.ttf", 32)
	farmer_key := rendering.load_and_save_texture("./assets/Images/farmer.png")
	schulz_texture_key := rendering.load_and_save_texture("./assets/Images/schulz.png")
    windmill_base_key := rendering.load_and_save_texture("./assets/Images/windmill_base.png")
    windmill_blades_key := rendering.load_and_save_texture("./assets/Images/windmill_blades.png")
	farmer := rendering.get_texture(farmer_key);

	anim_ref := AnimationRef.init(farmer, 39, 64, 4, true, true)
	anim := Animation.init(&anim_ref, 200)

	angle := 0
	ascending := false
	now: u32 := 0;
	last_new: u32 := 0;
	delta: f64 = 0;
	x := 0;
	y := 0;
    windmill_center := sdl.Point{180 / 2, 186 / 2}
    windmill_base := sdl.Rect{500, 100, 224 / 2, 463 / 2}
    windmill_blades := sld.Rect{514, 22, 338 / 2, 380 / 2}
    r := SDL_Rect{200, 200, 200, 200}
    r2 := SDL_Rect{800, 200, 200, 200}
    windmill_rotation: f64 = 0;
	no_touching_text: cstring = "No touching";
	no_touching_size: u8 = len(no_touching_text)
	hello_text: cstring = "Hello";
	hello_size: u8 = len(hello_text)
	no_touching_key: int = rendering.store_text_as_texture(no_touching_text, nullptr, WHITE);
	hello_key: int = rendering.store_text_as_texture(hello_text, nullptr, WHITE);

    astronaut_key: u64 = rendering.load_and_save_texture("./assets/Images/astronaut.png");

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

		// rendering.draw_stored_texture_rotated(schulz_texture_key, &r2, nullptr, angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
		rendering.draw_stored_text(hello_key, 1280 / 2, 720 / 2, 24, hello_size);
		rendering.draw_line(1280 / 2, 0, 1280 / 2, 720, WHITE);
		rendering.draw_line(0, 720 / 2, 1280, 720 / 2, WHITE);
		rendering.draw_rect(1280 / 2 - 60, 720 / 2 - 60, 120, 120, WHITE, false);
		rendering.draw_animation(&anim, 400, 100);
		rendering.draw_animation_rotated(&anim, 459, 100, angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);

        rendering.draw_stored_texture_rotated(astronaut_key, &r2, nullptr, angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);

        rendering.draw_stored_texture(windmill_base_key, &windmill_base, nullptr);
        rendering.draw_stored_texture_rotated(windmill_blades_key, &windmill_blades, nullptr, windmill_rotation, &windmill_center, SDL_RendererFlip::SDL_FLIP_NONE);
		rendering.show();

	//	if (ascending) {
			angle += (delta);
	//		if (angle > 30) ascending = !ascending;
	//	} else {
	//		angle -= (delta / 20);
	//		if (angle < -30) ascending = !ascending;
	//	}

        windmill_rotation += (delta / 10);
        if (windmill_rotation > 360.f) {
            windmill_rotation -= 360;
        }

		anim.update(delta);
	}
}
