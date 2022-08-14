package main

import "core:fmt"
import sdl "vendor:sdl2"
import ttf "vendor:sdl2/ttf"
import eng "engine"
import ren "rendering"
import inp "input"
import aud "audio"
import tests "tests"

WITH_TESTS :: true

when WITH_TESTS {

    main :: proc() {
        tests.input_test()
        // tests.rendering_test()
        // tests.audio_test()
    }

} else {

main :: proc() {
    fmt.println("hello")
    sdl.Init(sdl.INIT_VIDEO)
    window: ^sdl.Window
    renderer: ^sdl.Renderer
    sdl.CreateWindowAndRenderer(800, 600, sdl.WINDOW_RESIZABLE | sdl.WINDOW_ALLOW_HIGHDPI, &window, &renderer)
    sdl.SetRenderDrawColor(renderer, 0, 0, 0, sdl.ALPHA_OPAQUE)
    sdl.RenderClear(renderer)
    
    key_map: inp.KeyMap
    mouse_map: inp.MouseMap
    controller_map: inp.ControllerMap
    list: inp.ControllerList
    c := inp.initController()
    append(&list, c)
    input: inp.Input
    engine := eng.initEngine()
    inp.start_rebind_keyboard_action(engine.input, 0, sdl.Keycode.UNKNOWN)
    fmt.println(engine.input.escape_pressed)
    for inp.handle_input(engine.input) {}
    ttf.Quit()
    sdl.DestroyWindow(window)
    sdl.Quit()
}
}