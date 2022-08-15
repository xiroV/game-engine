package engine

import inp "../input"
import ren "../rendering"
import aud "../audio"

Engine :: struct {
    input: ^inp.Input,
    rendering: ^ren.Rendering,
    audio_manager: ^aud.AudioManager,
    delta: u64,
    quit: bool,
}

initEngineWithStandardParams :: proc() -> ^Engine {
    engine := new(Engine)
    engine.delta = 0
    engine.quit = false
    return engine
}

initEngineInputRendering :: proc(input: ^inp.Input, rendering: ^ren.Rendering, audio_manager: ^aud.AudioManager) -> ^Engine {
    engine := initEngineWithStandardParams()
    engine.input = input
    engine.rendering = rendering
    engine.audio_manager = audio_manager
    return engine
}

initEngineWithFields :: proc(initRenderingAndInput: bool) -> ^Engine {
    engine := initEngineWithStandardParams()
    if initRenderingAndInput {
        engine.rendering = ren.initRendering()
        engine.input = inp.initInput()
        engine.audio_manager = aud.initAudioManager()
    }
    return engine
}

get_components :: proc(engine: ^Engine) -> (^ren.Rendering, ^inp.Input, ^aud.AudioManager) {
    return engine.rendering, engine.input, engine.audio_manager
}

deinitEngine :: proc(engine: ^Engine) {
    if engine.input != nil do inp.deinitInput(engine.input)
    if engine.rendering != nil do ren.deinitRendering(engine.rendering)
    if engine.audio_manager != nil do aud.deinitAudioManager(engine.audio_manager)
    free(engine)
}

initEngine :: proc{initEngineInputRendering, initEngineWithFields, initEngineWithStandardParams}