package tests

import sdl "vendor:sdl2"
import ttf "vendor:sdl2/ttf"
import r "../rendering"
import i "../input"
import a "../audio"
import aniRef "../animation_reference"
import e "../engine"
import ani "../animation"
import fmt "core:fmt"
import math "core:math"

rendering_test :: proc() {
	rendering := r.initRendering()
	input := i.initInput()

	r.load_default_font(rendering, "./assets/Font/PressStart2P.ttf", 32)
	farmer_key := r.load_and_save_texture(rendering, "./assets/Images/farmer.png")
	schulz_texture_key := r.load_and_save_texture(rendering, "./assets/Images/schulz.png")
    windmill_base_key := r.load_and_save_texture(rendering, "./assets/Images/windmill_base.png")
    windmill_blades_key := r.load_and_save_texture(rendering, "./assets/Images/windmill_blades.png")
	farmer := r.get_texture(rendering, farmer_key);

	anim_ref, valid := aniRef.initAnimationReference(farmer, 39, 64, 4, true, true)
    if !valid do return 
    farmer_anim := ani.initAnimation(anim_ref, 200)

	angle: u32 = 0
	ascending := false
	now: u32 = 0;
	last_new: u32 = 0;
	delta: u32 = 0;
	x := 0;
	y := 0;
    windmill_center := sdl.Point{180 / 2, 186 / 2}
    windmill_base := sdl.Rect{500, 100, 224 / 2, 463 / 2}
    windmill_blades := sdl.Rect{514, 22, 338 / 2, 380 / 2}
    r1 := sdl.Rect{200, 200, 200, 200}
    r2 := sdl.Rect{800, 200, 200, 200}
    windmill_rotation: f64 = 0;
	no_touching_text: cstring = "No touching";
	no_touching_size := len(no_touching_text)
	hello_text: cstring = "Hello";
	hello_size := len(hello_text)
	no_touching_key  := r.store_text_as_texture(rendering, no_touching_text, nil, r.WHITE);
	hello_key := r.store_text_as_texture(rendering, hello_text, nil, r.WHITE);

    astronaut_key := r.load_and_save_texture(rendering, "./assets/Images/astronaut.png");

	for !i.handle_input(input) {
		x := input.mouse_position.x
		y := input.mouse_position.y
		now = sdl.GetTicks();
		delta = now - last_new
		last_new = now

		a: f64 = auto_cast abs(x - 300);
		b: f64 = auto_cast abs(y - 300);
		c: f64 = auto_cast math.sqrt(a * a + b * b)

		r.draw_stored_text(rendering, no_touching_key, r1.x + 20, r1.y + 100, 12, auto_cast no_touching_size);
        alpha: i32 = 0
        if c < 141 do a = 0
        else do a = 0 + clamp(c, 0, 255)
		r.set_texture_alpha(rendering, schulz_texture_key, auto_cast a);
		r.draw_stored_texture(rendering, schulz_texture_key, &r1);
		r.set_texture_alpha(rendering, schulz_texture_key, 255);

		// r.draw_stored_texture_rotated(schulz_texture_key, &r2, nullptr, angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
		r.draw_stored_text(rendering, hello_key, 1280 / 2, 720 / 2, 24, auto_cast hello_size);
		r.draw_line(rendering, 1280 / 2, 0, 1280 / 2, 720, r.WHITE);
		r.draw_line(rendering, 0, 720 / 2, 1280, 720 / 2, r.WHITE);
		r.draw_rect(rendering, 1280 / 2 - 60, 720 / 2 - 60, 120, 120, r.WHITE, false);
		r.draw_animation(rendering, farmer_anim, 400, 100);
		r.draw_animation_rotated(rendering, farmer_anim, 459, 100, auto_cast angle, nil, sdl.RendererFlip.NONE)

        r.draw_stored_texture_rotated(rendering, astronaut_key, &r2, nil, auto_cast angle, nil, sdl.RendererFlip.NONE)

        r.draw_stored_texture(rendering, windmill_base_key, &windmill_base, nil);
        r.draw_stored_texture_rotated(rendering, windmill_blades_key, &windmill_blades, nil, windmill_rotation, &windmill_center, sdl.RendererFlip.NONE);
		r.show(rendering);

	//	if (ascending) {
			angle += delta;
	//		if (angle > 30) ascending = !ascending;
	//	} else {
	//		angle -= (delta / 20);
	//		if (angle < -30) ascending = !ascending;
	//	}

        windmill_rotation += auto_cast (delta / 10.0);
        if windmill_rotation > 360 {
            windmill_rotation -= 360;
        }

		ani.update(farmer_anim, delta);
	}
}

audio_test :: proc() {
	audio := a.initAudioManager()
	rendering := r.initRendering()
	input := i.initInput()
	r.load_default_font(rendering, "./assets/Font/PressStart2P.ttf", 32)
	boing_audio_id := a.load_and_save_sound_effect(audio, "./assets/Sounds/boing.wav")
	success, elevator_music_audio_id := a.load_and_save_music(audio, "./assets/Sounds/beat.wav")
	fmt.println(boing_audio_id)
	fmt.println(elevator_music_audio_id)
	for !i.handle_input(input) {
		
		r.draw_text(rendering, "Press X to boing", r.WHITE, 25, 25, 25, nil);
		r.draw_text(rendering, "Press D to play smooth muzak.", r.WHITE, 25, 25, 55, nil);
		r.show(rendering);

		if i.is_key_pressed(input, SDLK_x) {
			a.play_sound(boing_audio_id)
		}

		if i.is_key_pressed(input, SDLK_d) {
			if (a.is_music_playing()) {
				if a.is_music_paused(audio) do a.resume_music(audio)
				else do a.pause_music(audio)
			} else {
				a.play_music(audio, elevator_music_audio_id, -1);
			}
		}
	}
}

TestState :: struct {
    jump_toggled: bool,
    left_toggled: bool,
    right_toggled: bool,
    attack_toggled: bool,
};

UserAction :: enum i32 {
    MoveLeft = 1,
    MoveRight,
    Jump,
    Attack,
    Select
}

MAX_KEYS_PER_ACTION :: 2;

input_test :: proc() {
    sdl.Init(sdl.INIT_VIDEO | sdl.INIT_GAMECONTROLLER | sdl.INIT_JOYSTICK)
    test_state: TestState
    ttf.Init();
    
    font := ttf.OpenFont(".\\assets\\Font\\PressStart2P.ttf", 32);

    if font == nil {
        fmt.println("Failed to load font")
        fmt.println(ttf.GetError())
        return 1;
    }

    window: ^sdl.Window;
    renderer: ^sdl.Renderer;
    sdl.CreateWindowAndRenderer(1280, 720, sdl.WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    sdl.RenderSetLogicalSize(renderer, 1280, 720);    
    sdl.SetRenderDrawColor(renderer, 0, 0, 0, sdl.ALPHA_OPAQUE);
    sdl.RenderClear(renderer);

    mouse_map: i.MouseMap ;
    mouse_map[sdl.BUTTON_LEFT] = UserAction.Select;

    controller_map: i.ControllerMap
    controller_map[sdl.GameControllerButton.B] = UserAction.Attack;
    controller_map[sdl.GameControllerButton.A] = UserAction.Jump;
    
    input: Input

    i.bind_key_to_action(input, sdl.Keycode.LEFT, UserAction.MoveLeft);
    i.bind_key_to_action(input, sdl.Keycode.LEFT, UserAction.MoveLeft);
    i.bind_key_to_action(input, sdl.Keycode.SPACE,UserAction.Jump);
    i.bind_key_to_action(input, sdl.Keycode.LCTRL,UserAction.Attack);

    bind_mouse_button_to_action(input, sdl.BUTTON_LEFT, Select);

    bind_controller_button_to_action(input, sdl.GameControllerButton.A, UserAction.Attack, 0);
    bind_controller_button_to_action(input, sdl.GameControllerButton.B, UserAction.Jump, 0);

    engine := e.initEngine(&input, renderer)

    for !engine.input.handle_input() {
        if (engine.input.is_pressed(UserAction.Jump, true, 0)) {
            test_state.jump_toggled = !test_state.jump_toggled;
        }

        if (engine.input.is_pressed(UserAction.Attack, true, 0)) {
            test_state.attack_toggled = !test_state.attack_toggled;
        }

        if (engine.input.is_pressed(UserAction.MoveLeft, true, 0)) {
            test_state.left_toggled = !test_state.left_toggled;
        }

        if (engine.input.is_pressed(UserAction.MoveRight, true, 0)) {
            test_state.right_toggled = !test_state.right_toggled;
        }

        mouseCoordinates := engine.input.mouse_position;
        mouseDelta := engine.input.mouse_delta;
        mouseWheel := engine.input.mouse_wheel;


        // Controller info
        r.render_text(renderer, font, "Controllers connected: " + to_string(sdl.NumJoysticks()), r.WHITE, 25, 625, 5);
        j := 0;
        for controller in engine.input.controllers {
            x_column: u8 = j % 2;
            y_row: u8 = j / 2;
            r.render_text(renderer, font, "Controller " + to_string(j), r.WHITE, 12, 625 + x_column * 330, 35 + 100 * y_row);
            r.render_text(renderer, font, "Left horizontal: " + to_string(controller.left.horizontal_axis), r.WHITE, 12, 625 + x_column * 330, 50 + 100 * y_row);
            r.render_text(renderer, font, "Left vertical: " + to_string(controller.left.vertical_axis), r.WHITE, 12, 625 + x_column * 330, 70 + 100 * y_row);
            r.render_text(renderer, font, "Right horizontal: " + to_string(controller.right.horizontal_axis), r.WHITE, 12, 625 + x_column * 330, 90 + 100 * y_row);
            r.render_text(renderer, font, "Right vertical: " + to_string(controller.right.vertical_axis), r.WHITE, 12, 625 + x_column * 330, 110 + 100 * y_row);
            r.render_text(renderer, font, "Left trigger: " + to_string(controller.left_trigger), r.WHITE, 12, 625 + x_column * 330, 130 + 100 * y_row);
            r.render_text(renderer, font, "Right trigger: " + to_string(controller.right_trigger), r.WHITE, 12, 625 + x_column * 330, 150 + 100 * y_row);
            j += 1;
        }

        // For each action, show bound key;
        // On click, set rebind for a key.
        r.render_text(renderer, font, "Jump toggled " + bool_string(test_state.jump_toggled), WHITE, 25, 10, 5);
        r.render_text(renderer, font, "Attack toggled " + bool_string(test_state.attack_toggled), WHITE, 25, 10, 40);
        r.render_text(renderer, font, "Left toggled " + bool_string(test_state.left_toggled), WHITE, 25, 10, 75);
        r.render_text(renderer, font, "Right toggled " + bool_string(test_state.right_toggled), WHITE, 25, 10, 110);

        r.render_text(renderer, font, "Jump " + up_or_down(engine.input.is_held(UserAction.Jump, true, 0)), WHITE, 25, 10, 145);
        r.render_text(renderer, font, "Attack " + up_or_down(engine.input.is_held(UserAction.Attack, true, 0)), WHITE, 25, 10, 180);
        r.render_text(renderer, font, "Left " + up_or_down(engine.input.is_held(UserAction.MoveLeft, true, 0)), WHITE, 25, 10, 215);
        r.render_text(renderer, font, "Right " + up_or_down(engine.input.is_held(UserAction.MoveRight, true, 0)), WHITE, 25, 10, 250);
        r.render_text(renderer, font, "Mouse pos x: " + to_string(mouseCoordinates.x), WHITE, 25, 10, 285);
        r.render_text(renderer, font, "Mouse pos y: " + to_string(mouseCoordinates.y), WHITE, 25, 10, 320);
        r.render_text(renderer, font, "Mouse delta x: " + to_string(mouseDelta.x), WHITE, 25, 10, 355);
        r.render_text(renderer, font, "Mouse delta y: " + to_string(mouseDelta.y), WHITE, 25, 10, 390);
        r.render_text(renderer, font, "Mouse scroll x: " + to_string(mouseWheel.x), WHITE, 25, 10, 425);
        r.render_text(renderer, font, "Mouse scroll y: " + to_string(mouseWheel.y), WHITE, 25, 10, 465);
        r.render_text(renderer, font, "Action", GREEN, 25, 10, 500);
        r.render_text(renderer, font, "Key", GREEN, 25, 250, 500);
        r.render_text(renderer, font, "Key 2", GREEN, 25, 500, 500);
        r.render_text(renderer, font, "Controller", GREEN, 25, 750, 500);

        i := 0;
        sdl.SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for act := 1; act < auto_cast UserAction.Select; act += 1 {
            action_name := useraction_to_name(auto_cast act);
            mapped_count := MAX_KEYS_PER_ACTION;
            y := 535 + 35 * i;
            r.render_text(renderer, font, action_name, WHITE, 25, 10, y);
            j := 0;
            for key_pair in engine.input.key_map {
                if key_pair.second != act || mapped_count == 0 do continue;
                mapped_count -= 1
                rect := sdl.Rect{ 250 - 4 + j * 250, y - 4, 240, 32 };
                sdl.RenderDrawRect(renderer, &rect);
                r.render_text(renderer, font, sdl.GetKeyName(key_pair.first), WHITE, 25, 250 + j * 250, y);
                j += 1;
                if (sdl.PointInRect(&mouseCoordinates, &rect) && engine.input.is_mouse_button_pressed(sdl.BUTTON_LEFT)) {
                    fmt.println("Preparing to rebind ", action_name, " with intention to erase ", sdl.GetKeyName(key_pair.first))
                    engine.input.start_rebind_keyboard_action(auto_cast act, key_pair.first);
                }
            }

            for mouse_pair in engine.input.mouse_map {
                if mouse_pair.second != act || mapped_count == 0 do continue;
                mapped_count -= 1;
                rect:= sdl.Rect { 250 - 4 + j * 250, y - 4, 240, 32 };
                sdl.RenderDrawRect(renderer, &rect);
                render_text(renderer, font, mouse_button_to_name(mouse_pair.first), WHITE, 25, 250 + j * 250, y);
                j += 1;
                if (sdl.PointInRect(&mouseCoordinates, &rect) && engine.input.is_mouse_button_pressed(sdl.BUTTON_LEFT)) {
                    engine.input.start_rebind_mouse_action(auto_cast act, auto_cast mouse_pair.first);
                }
            }

            if (mapped_count) {
                for ; mapped_count > 0; mapped_count -= 1 {
                    r.render_text(renderer, font, "Unmapped", WHITE, 25, 250 + j * 250, y);
                    rect := sdl.Rect { 250 - 4 + j * 250, y - 4, 240, 32 };
                    sdl.RenderDrawRect(renderer, &rect);
                    j += 1;

                    if (sdl.PointInRect(&mouseCoordinates, &rect) && engine.input.is_mouse_button_pressed(sdl.BUTTON_LEFT)) {
                        fmt.println("Preparing to rebind ", action_name, " with intention to write to unmapped.")
                        engine.input.start_rebind_keyboard_action(auto_cast act, SDLK_UNKNOWN);
                    }
                }
            }

            button := -1;

            for controller_pair in engine.input.controllers[0].controller_map {
                if (controller_pair.second == act) {
                    button = auto_cast controller_pair.first;
                }
            }

            
            button_text: cstring;
            if button == -1 { 
                button_text = "Unmapped" 
            } else { 
                button_text = sdl.GameControllerGetStringForButton(auto_cast button)
            }
            render_text(renderer, font, button_text, WHITE, 25, 250 + j * 250, y);
            rect := sdl.Rect { 250 - 4 + j * 250, y - 4, 240, 32 };
            sdl.RenderDrawRect(renderer, &rect);
            j += 1;

            if sdl.PointInRect(&mouseCoordinates, &rect) && engine.input.is_mouse_button_pressed(sdl.MouseButton.LEFT) {
                engine.input.start_rebind_action_controller(auto_cast act, 0, button);
            }

            i += 1;
        }

        sdl.SetRenderDrawColor(renderer, 0, 0, 0, 255);

        sdl.RenderPresent(renderer);
        sdl.RenderClear(renderer);
    }

    ttf.CloseFont(font);
    ttf.Quit();
    sdl.DestroyWindow(window);
    sdl.Quit();
    return 0;
}

mouse_button_to_name :: proc(button: u8) -> cstring {
    switch button {
        case sdl.BUTTON_LEFT:
            return "Mouse Left";
        case sdl.BUTTON_RIGHT:
            return "Mouse Right";
        case sdl.BUTTON_MIDDLE:
            return "Mouse Middle";
        case sdl.BUTTON_X1:
            return "Mouse X1";
        case sdl.BUTTON_X2:
            return "Mouse X2";
        case:
            return "Unhandled mouse button";
    }
}

up_or_down :: proc(value: bool) -> cstring {
    if value do return "down"
    else do return "up";
}

bool_string :: proc(value: bool) -> cstring {
    if value do return "true"
    else do return "false"
}

useraction_to_name :: proc(a: i32) -> cstring {
    switch a {
        case UserAction.MoveLeft:
            return "MoveLeft";
        case UserAction.MoveRight:
            return "MoveRight";
        case UserAction.Jump:
            return "Jump";
        case UserAction.Attack:
            return "Attack";
        case:
            return "Not mapped";
    }
}
