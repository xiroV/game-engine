package tests

import c "core:c"
import libc "core:c/libc"
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

to_string :: proc(foo: $T) -> cstring {
    return "to_string TODO"
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

		if i.is_key_pressed(input, sdl.Keycode.x) {
			a.play_sound(audio, boing_audio_id)
		}

		if i.is_key_pressed(input, sdl.Keycode.d) {
			if (a.is_music_playing()) {
				if a.is_music_paused() do a.resume_music()
				else do a.pause_music()
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
    test_state: TestState
    
    font := ttf.OpenFont(".\\assets\\Font\\PressStart2P.ttf", 32);
    defer {
        ttf.CloseFont(font)
        ttf.Quit()
    }

    if font == nil {
        fmt.println("Failed to load font")
        fmt.println(ttf.GetError())
        return;
    }

    window: ^sdl.Window;
    renderer: ^sdl.Renderer;
    sdl.CreateWindowAndRenderer(1280, 720, sdl.WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    sdl.RenderSetLogicalSize(renderer, 1280, 720);    
    sdl.SetRenderDrawColor(renderer, 0, 0, 0, sdl.ALPHA_OPAQUE);
    sdl.RenderClear(renderer);


    input := i.initInput()
    input.controllers[0].controller_map[sdl.GameControllerButton.B] = auto_cast UserAction.Attack;
    input.controllers[0].controller_map[sdl.GameControllerButton.A] = auto_cast UserAction.Jump;
    input.mouse_map[sdl.BUTTON_LEFT] = auto_cast UserAction.Select;
    
    rendering := r.initRendering()

    i.bind_key_to_action(input, sdl.Keycode.LEFT, auto_cast UserAction.MoveLeft);
    i.bind_key_to_action(input, sdl.Keycode.LEFT, auto_cast UserAction.MoveLeft);
    i.bind_key_to_action(input, sdl.Keycode.SPACE, auto_cast UserAction.Jump);
    i.bind_key_to_action(input, sdl.Keycode.LCTRL, auto_cast UserAction.Attack);

    i.bind_mouse_button_to_action(input, sdl.BUTTON_LEFT, auto_cast UserAction.Select);

    i.bind_controller_button_to_action(input, sdl.GameControllerButton.A, auto_cast UserAction.Attack, 0);
    i.bind_controller_button_to_action(input, sdl.GameControllerButton.B, auto_cast UserAction.Jump, 0);

    engine := e.initEngine(input, rendering)

    for !i.handle_input(engine.input) {
        if (i.is_pressed(engine.input, auto_cast UserAction.Jump, true, 0)) {
            test_state.jump_toggled = !test_state.jump_toggled;
        }

        if (i.is_pressed(engine.input, auto_cast UserAction.Attack, true, 0)) {
            test_state.attack_toggled = !test_state.attack_toggled;
        }

        if (i.is_pressed(engine.input, auto_cast UserAction.MoveLeft, true, 0)) {
            test_state.left_toggled = !test_state.left_toggled;
        }

        if (i.is_pressed(engine.input, auto_cast UserAction.MoveRight, true, 0)) {
            test_state.right_toggled = !test_state.right_toggled;
        }

        mouseCoordinates := engine.input.mouse_position;
        mouseDelta := engine.input.mouse_delta;
        mouseWheel := engine.input.mouse_wheel;


        // Controller info
        
        text := fmt.aprintf("Controllers connected: %i\n", sdl.NumJoysticks());

        r.draw_text(engine.rendering, text, r.WHITE, 25, 625, 5, font);
        j := 0;
        for controller in engine.input.controllers {
            x_column := j % 2;
            y_row := j / 2;
            r.draw_text(engine.rendering, fmt.aprintf("Controller %i", j), r.WHITE, 12, 625 + x_column * 330, 35 + 100 * y_row, font)
            r.draw_text(engine.rendering, fmt.aprintf("Left horizontal: %i", controller.left.horizontal_axis), r.WHITE, 12, 625 + x_column * 330, 50 + 100 * y_row, font)
            r.draw_text(engine.rendering, fmt.aprintf("Left vertical: ", controller.left.vertical_axis), r.WHITE, 12, 625 + x_column * 330, 70 + 100 * y_row, font)
            r.draw_text(engine.rendering, fmt.aprintf("Right horizontal: %i", controller.right.horizontal_axis), r.WHITE, 12, 625 + x_column * 330, 90 + 100 * y_row, font)
            r.draw_text(engine.rendering, fmt.aprintf("Right vertical: %i", controller.right.vertical_axis), r.WHITE, 12, 625 + x_column * 330, 110 + 100 * y_row, font)
            r.draw_text(engine.rendering, fmt.aprintf("Left trigger: %i", controller.left_trigger), r.WHITE, 12, 625 + x_column * 330, 130 + 100 * y_row, font)
            r.draw_text(engine.rendering, fmt.aprintf("Right trigger: %i", controller.right_trigger), r.WHITE, 12, 625 + x_column * 330, 150 + 100 * y_row, font)
            j += 1;
        }

        // For each action, show bound key;
        // On click, set rebind for a key.
        r.draw_text(engine.rendering, fmt.aprintf("Jump toggled %b", bool_string(test_state.jump_toggled)), r.WHITE, 25, 10, 5, font)
        r.draw_text(engine.rendering, fmt.aprintf("Attack toggled %b", bool_string(test_state.attack_toggled)), r.WHITE, 25, 10, 40, font)
        r.draw_text(engine.rendering, fmt.aprintf("Left toggled %b", bool_string(test_state.left_toggled)), r.WHITE, 25, 10, 75, font)
        r.draw_text(engine.rendering, fmt.aprintf("Right toggled %b", bool_string(test_state.right_toggled)), r.WHITE, 25, 10, 110, font);

        r.draw_text(engine.rendering, fmt.aprintf("Jump %s", up_or_down(i.is_held(engine.input, auto_cast UserAction.Jump, true, 0))), r.WHITE, 25, 10, 145, font)
        r.draw_text(engine.rendering, fmt.aprintf("Attack %s", up_or_down(i.is_held(engine.input, auto_cast UserAction.Attack, true, 0))), r.WHITE, 25, 10, 180, font)
        r.draw_text(engine.rendering, fmt.aprintf("Left %s", up_or_down(i.is_held(engine.input, auto_cast UserAction.MoveLeft, true, 0))), r.WHITE, 25, 10, 215, font)
        r.draw_text(engine.rendering, fmt.aprintf("Right %s", up_or_down(i.is_held(engine.input, auto_cast UserAction.MoveRight, true, 0))), r.WHITE, 25, 10, 250, font)
        r.draw_text(engine.rendering, fmt.aprintf("Mouse pos x: %i", mouseCoordinates.x), r.WHITE, 25, 10, 285, font)
        r.draw_text(engine.rendering, fmt.aprintf("Mouse pos y: %i", mouseCoordinates.y), r.WHITE, 25, 10, 320, font)
        r.draw_text(engine.rendering, fmt.aprintf("Mouse delta x: %i", mouseDelta.x), r.WHITE, 25, 10, 355, font)
        r.draw_text(engine.rendering, fmt.aprintf("Mouse delta y: %i", mouseDelta.y), r.WHITE, 25, 10, 390, font)
        r.draw_text(engine.rendering, fmt.aprintf("Mouse scroll x: %i", mouseWheel.x), r.WHITE, 25, 10, 425, font)
        r.draw_text(engine.rendering, fmt.aprintf("Mouse scroll y: %i", mouseWheel.y), r.WHITE, 25, 10, 465, font)
        r.draw_text(engine.rendering, "Action", r.GREEN, 25, 10, 500, font)
        r.draw_text(engine.rendering, "Key", r.GREEN, 25, 250, 500, font)
        r.draw_text(engine.rendering, "Key 2", r.GREEN, 25, 500, 500, font)
        r.draw_text(engine.rendering, "Controller", r.GREEN, 25, 750, 500, font)

        k := 0;
        sdl.SetRenderDrawColor(engine.rendering.renderer, 255, 255, 255, 255);
        for act in UserAction {
            action_name := useraction_to_name(act)
            mapped_count := MAX_KEYS_PER_ACTION
            y := 535 + 35 * k
            r.draw_text(engine.rendering, action_name, r.WHITE, 25, 10, y, font)
            j := 0
            for key, value in engine.input.key_map {
                if value != auto_cast act || mapped_count == 0 do continue;
                mapped_count -= 1
                rect := sdl.Rect{ auto_cast (250 - 4 + j * 250), auto_cast (y - 4), 240, 32 };
                sdl.RenderDrawRect(engine.rendering.renderer, &rect);
                r.draw_text(engine.rendering, string(sdl.GetKeyName(key)), r.WHITE, 25, 250 + j * 250, y, font);
                j += 1;
                if (sdl.PointInRect(&mouseCoordinates, &rect) && i.is_mouse_button_pressed(engine.input, sdl.BUTTON_LEFT)) {
                    fmt.println("Preparing to rebind ", action_name, " with intention to erase ", string(sdl.GetKeyName(key)))
                    i.start_rebind_keyboard_action(engine.input, auto_cast act, key);
                }
            }

            for key, value in engine.input.mouse_map {
                if value != auto_cast act || mapped_count == 0 do continue;
                mapped_count -= 1;
                rect:= sdl.Rect { auto_cast (250 - 4 + j * 250), auto_cast (y - 4), 240, 32 };
                sdl.RenderDrawRect(renderer, &rect);
                r.draw_text(engine.rendering, mouse_button_to_name(key), r.WHITE, 25, 250 + j * 250, y, font);
                j += 1;
                if (sdl.PointInRect(&mouseCoordinates, &rect) && i.is_mouse_button_pressed(engine.input, sdl.BUTTON_LEFT)) {
                    i.start_rebind_mouse_action(engine.input, auto_cast act, auto_cast key);
                }
            }

            if (mapped_count > 0) {
                for ; mapped_count > 0; mapped_count -= 1 {
                    r.draw_text(engine.rendering, "Unmapped", r.WHITE, 25, 250 + j * 250, y, font);
                    rect := sdl.Rect { auto_cast (250 - 4 + j * 250), auto_cast (y - 4), 240, 32 };
                    sdl.RenderDrawRect(renderer, &rect);
                    j += 1;

                    if (sdl.PointInRect(&mouseCoordinates, &rect) && i.is_mouse_button_pressed(engine.input, sdl.BUTTON_LEFT)) {
                        fmt.println("Preparing to rebind ", action_name, " with intention to write to unmapped.")
                        i.start_rebind_keyboard_action(engine.input, auto_cast act, sdl.Keycode.UNKNOWN);
                    }
                }
            }

            button := sdl.GameControllerButton.INVALID
            
            for key, value in engine.input.controllers[0].controller_map {
                if (value == auto_cast act) {
                    button = auto_cast key
                }
            }

            
            button_text: string
            if button == sdl.GameControllerButton.INVALID { 
                button_text = "Unmapped" 
            } else { 
                button_text = string(sdl.GameControllerGetStringForButton(auto_cast button))
            }
            r.draw_text(engine.rendering, button_text, r.WHITE, 25, 250 + j * 250, y, font)
            rect := sdl.Rect { auto_cast (250 - 4 + j * 250), auto_cast (y - 4), 240, 32 }
            sdl.RenderDrawRect(renderer, &rect);
            j += 1;

            if sdl.PointInRect(&mouseCoordinates, &rect) && i.is_mouse_button_pressed(engine.input, sdl.BUTTON_LEFT) {
                i.start_rebind_action_controller(engine.input, auto_cast act, 0, button);
            }

            k += 1;
        }

        sdl.SetRenderDrawColor(renderer, 0, 0, 0, 255);

        sdl.RenderPresent(renderer);
        sdl.RenderClear(renderer);
    }
}

mouse_button_to_name :: proc(button: u8) -> string {
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

up_or_down :: proc(value: bool) -> string {
    if value do return "down"
    else do return "up";
}

bool_string :: proc(value: bool) -> string {
    if value do return "true"
    else do return "false"
}

useraction_to_name :: proc(a: UserAction) -> string {
    switch a {
        case UserAction.MoveLeft:
            return "MoveLeft";
        case UserAction.MoveRight:
            return "MoveRight";
        case UserAction.Jump:
            return "Jump";
        case UserAction.Attack:
            return "Attack";
        case UserAction.Select:
            return "Select"
        case:
            return "Not mapped";
    }
}
