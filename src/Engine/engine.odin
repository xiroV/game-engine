package engine

import inp "../input"
import ren "../rendering"

Engine :: struct {
    input: ^inp.Input,
    rendering: ^ren.Rendering,
    delta: u64,
    quit: bool,
}

initEngineWithStandardParams :: proc() -> ^Engine {
    engine := new(Engine)
    engine.delta = 0
    engine.quit = false
    return engine
}

initEngineInputRendering :: proc(input: ^inp.Input, rendering: ^ren.Rendering) -> ^Engine {
    engine := initEngineWithStandardParams()
    engine.input = input
    engine.rendering = rendering
    return engine
}

initEngineWithFields :: proc(initRenderingAndInput: bool) -> ^Engine {
    engine := initEngineWithStandardParams()
    if initRenderingAndInput {
        engine.rendering = ren.initRendering()
        engine.input = inp.initInput()
    }
    return engine
}

initEngine :: proc{initEngineInputRendering, initEngineWithFields, initEngineWithStandardParams}