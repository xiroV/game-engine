package main

import "core:fmt"
import sdl "vendor:sdl2"
import ttf "vendor:sdl2/ttf"
import rendering "rendering"

WITH_TESTS :: false


when WITH_TESTS {

    main :: proc() {
        // input_test();
        // rendering_test();
        // audio_test();
    }

} else {

main :: proc() {
    sdl.Init(sdl.INIT_VIDEO);
    window: ^sdl.Window;
    renderer: ^sdl.Renderer;
    sdl.CreateWindowAndRenderer(800, 600, sdl.WINDOW_RESIZABLE | sdl.WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    sdl.SetRenderDrawColor(renderer, 0, 0, 0, sdl.ALPHA_OPAQUE);
    sdl.RenderClear(renderer);
    
    key_map: KeyMap
    mouse_map: MouseMap
    controller_map: ControllerMap
    list: ControllerList
    c := Controller{
        false,
        false,
        {0, 0},
        {0, 0},
        0,
        0,
        controller_map
    }
    list.emplace_back(c)
    input: Input
    engine: Engine(input)
    engine.input.start_rebind_keyboard_action(0, Keycode.KeyCode.UNKNOWN);
    running := true
    for !engine.input.handle_input() {}
    ttf.Quit();
    sdl.DestroyWindow(window);
    sdl.Quit();
}
}