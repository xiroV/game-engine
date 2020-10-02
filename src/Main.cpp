#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>

#include "Input/Input.hpp"
#include "Input/Input_test.hpp"
#include "Rendering/Rendering_test.hpp"
#include "Engine.h"

#if defined(_WIN32)
#define MAIN_NAME WinMain
#else
#define MAIN_NAME main
#endif

#if WITH_TESTS

int MAIN_NAME(int argc, char* argv[]) {
    input_test();
    rendering_test();
    return 0;
}

#else 

int MAIN_NAME(int argc, char* argv[]) {
    if (TTF_Init() < 0) {
        std::cout << "Failed to initialize TTF" << std::endl;
    }   
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    KeyMap key_map;
    MouseMap mouse_map;
    ControllerMap controller_map;
    ControllerList list;
    Controller c = {
        false,
        false,
        {0, 0},
        {0, 0},
        0,
        0,
        controller_map
    };
    list.emplace_back(c);
    Input input(key_map, mouse_map, list);
    Engine engine(input);
    engine.input.start_rebind_keyboard_action(Jump, SDLK_UNKNOWN);
    bool running = true;
    while (!engine.input.handle_input()) {}
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
#endif