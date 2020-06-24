#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>

#include "Input/Input.hpp"
#include "Input/Input_test.hpp"
#include "Engine.h"

#if WITH_TESTS

int main(int argc, char* argv[]) {
    bool any_test = false;
    input_test();
    return 0;
}

#else 

int main(int argc, char* argv[]) {
    if (TTF_Init() < 0) {
        std::cout << "Failed to initialize TTF" << std::endl;
    }   
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    std::cout << "Finished binding keys" << std::endl;
    
    KeyMap keys;
    MouseMap mouse;
    ControllerMap controller;
    Input input(keys, mouse, controller);
    Engine engine(input);
    engine.input.set_action_to_rebind(Jump, RebindingDevice::KeyboardAndMouse, SDLK_UNKNOWN);
    bool running = true;
    while (running) {
        running = !engine.input.handle_input();
    }

    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
#endif